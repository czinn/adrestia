#include "monte_strategy.h"
#include <chrono>
#include <cmath>
#include <algorithm>
#include <vector>
#include <deque>
#include <set>
#include <iostream>
#include "game_action.h"
#include "game_view.h"
#include "game_state.h"

struct TreeNode {
  int total;
  bool updated;
  std::vector<GameAction> actions;
  std::vector<double> regret;
  std::vector<double> strategy;
  std::vector<int> visits;
  std::vector<double> utility;
};

MonteStrategy::MonteStrategy(int iterations, bool multilevel)
    : gen(std::chrono::high_resolution_clock::now().time_since_epoch().count())
    , iterations(iterations)
    , multilevel(multilevel) {}

MonteStrategy::~MonteStrategy() {}

size_t get_view_hash(const GameView &v) {
  // TODO: charles: Choose a better view hash. Right now this is slow and
  // probably bad, but is fast to code and probably works.
  // Hash must, at minimum, include (explicitly or implicitly) mana and tech
  // for current player.
  size_t pid = v.view_player_id;
  const Player &p = v.players[pid];
  size_t h = (p.mp << 0) + (p.mp_regen << 4) + (p.hp << 8) + (v.players[1 - pid].hp << 13) + (v.turn_number() << 18);
  for (size_t i = 0; i < p.tech.size(); i++) {
    h += std::hash<std::string>{}(p.books[i]->get_id()) * (1 + p.tech[i]);
  }
  return h;
  //return std::hash<std::string>{}(json(v.view_player_id).dump() + json(v.history).dump());
}

// Returns a random set containing k integers in the range [0, n-1]
std::set<int> choose(std::mt19937 &gen, int n, int k) {
  std::set<int> r;
  while (r.size() < k) {
    r.insert(gen() % n);
  }
  return r;
}

struct PathNode {
  TreeNode *tree_node;
  int chosen_child;
};

GameAction MonteStrategy::get_action(const GameView &view) {
  const Player &view_player = view.players[view.view_player_id];
  // Determine which techs are possible for the opponent.
  std::map<std::string, int> visible_tech[view.players.size()];
  int unknown_techs[view.players.size()];
  for (int i = 0; i < view.players.size(); i++) {
    unknown_techs[i] = 0;
  }
  for (const auto &turn : view.history) {
    for (int i = 0; i < view.players.size(); i++) {
      for (const auto &spell_id : turn[i]) {
        if (spell_id == TECH_SPELL_ID) {
          unknown_techs[i]++;
          continue;
        }
        const Spell &spell = view.rules.get_spell(spell_id);
        if (spell.is_tech_spell()) {
          unknown_techs[i]++;
        }
        auto it = visible_tech[i].find(spell.get_book());
        if (it == visible_tech[i].end()) {
          visible_tech[i][spell.get_book()] = spell.get_tech();
        } else {
          it->second = std::max(it->second, spell.get_tech());
        }
      }
    }
  }
  int number_of_books = view_player.books.size();
  std::vector<int> known_tech[view.players.size()];
  std::vector<const Book*> known_books[view.players.size()];
  std::vector<std::string> remaining_books[view.players.size()];
  for (int i = 0; i < view.players.size(); i++) {
    std::transform(
        view.rules.get_books().begin(),
        view.rules.get_books().end(),
        std::back_inserter(remaining_books[i]),
        [](const std::map<std::string,Book>::value_type &pair){return pair.first;});
    for (const auto &[k, v] : visible_tech[i]) {
      unknown_techs[i] -= v;
      known_tech[i].push_back(v);
      known_books[i].push_back(&view.rules.get_book(k));
      remaining_books[i].erase(std::find(remaining_books[i].begin(),
            remaining_books[i].end(), k));
    }
  }

  // Iterations!
  std::map<size_t, TreeNode> tree;
  std::uniform_real_distribution<double> distribution(0.0,1.0);
  for (int i = 0; i < iterations; i++) {
    // Choose the determinization
    // For now, don't do self-determinizations.
    size_t op = 1 - view.view_player_id;
    std::vector<int> tech = known_tech[op];
    std::vector<const Book*> books = known_books[op];
    std::set<int> book_selection =
      choose(gen, remaining_books[op].size(), number_of_books - books.size());
    for (const auto book : book_selection) {
      books.push_back(&view.rules.get_book(remaining_books[op][book]));
    }
    {
      std::set<int> tech_distribution =
        choose(gen, unknown_techs[op] + books.size() - 1, unknown_techs[op]);
      int last_distr = -1;
      int to_push = 0;
      while (tech.size() < books.size()) {
        tech.push_back(0);
      }
      int j = 0;
      for (const auto t : tech_distribution) {
        if (t == last_distr + 1) {
          to_push++;
        } else {
          tech[j++] += to_push;
          to_push = 0;
        }
        last_distr = t;
      }
      tech[j] += to_push;
    }
    // Create the determinization.
    GameState g(view, tech, books);
    // Expand (i.e. add to the tree) only one node per player per iteration.
    bool expanded[view.players.size()];
    expanded[0] = false;
    expanded[1] = false;
    std::deque<PathNode> paths[view.players.size()];
    int turn_number = 0;
    // While the game isn't finished, look up states in the tree for each player and pick actions.
    while (g.winners().size() == 0) {
      std::vector<GameAction> the_turn;
      for (int i = 0; i < view.players.size(); i++) {
        GameView v(g, i);
        size_t view_hash = get_view_hash(v);
        // If we haven't expanded this node...
        if (tree.count(view_hash) == 0) {
          // ...and we haven't expanded a node for this player yet this iteration...
          if (!expanded[i] && (turn_number == 0 || multilevel)) {
            // ...expand the node.
            expanded[i] = true;
            std::vector<GameAction> actions = v.legal_actions();
            tree[view_hash] = TreeNode
              { 0
              , false
              , actions
              , std::vector<double>(actions.size())
              , std::vector<double>(actions.size())
              , std::vector<int>(actions.size())
              , std::vector<double>(actions.size())
              };
            for (int j = 0; j < actions.size(); j++) {
              tree[view_hash].regret[i] = 0.0;
              tree[view_hash].strategy[i] = 0.0;
              tree[view_hash].visits[i] = 0;
              tree[view_hash].utility[i] = 0.0;
            }
          } else {
            // Otherwise, choose a random action.
            the_turn.push_back(v.random_action(gen));
            continue;
          }
        }
        // Node has been expanded (either previously or just now).
        TreeNode &node = tree[view_hash];
        node.updated = false;
        // If not all children have been tried yet...
        int chosen_child = 0;
        int N = node.actions.size();
        double gamma = 0.1; // std::min(1.0, sqrt(A * log(A) / (exp(1) - 1) / n));
        if (node.total < N) {
          // ...choose a random untried child, and don't expand this iteration.
          expanded[i] = true;
          int untried_children = 0;
          for (int j = 0; j < N; j++) {
            if (node.visits[j] == 0) {
              untried_children++;
              if (gen() % untried_children == 0) {
                chosen_child = j;
              }
            }
          }
        } else {
          // Otherwise, choose a node based on regret matching
          double p[N];
          double total_regret = 0;
          for (int j = 0; j < N; j++) {
            total_regret += std::max(0.0, node.regret[j]);
            p[j] = std::max(0.0, node.regret[j]);
          }
          double selection = distribution(gen);
          for (int j = 0; j < N; j++) {
            if (total_regret > 0) {
              p[j] /= total_regret;
            } else {
              p[j] = 1.0 / N;
            }
            node.strategy[j] += p[j];
            selection -= p[j] * (1 - gamma) + gamma / N;
            if (selection <= 0) {
              chosen_child = j;
              selection += 2;
            }
          }
        }
        the_turn.push_back(node.actions[chosen_child]);
        paths[i].push_back(PathNode { &node, chosen_child });
      }
      g.simulate(the_turn);
      turn_number++;
      if (turn_number > 10) {
        break;
      }
    }
    // Update node statistics.
    const std::vector<size_t> &winners = g.winners();
    for (size_t i = 0; i < view.players.size(); i++) {
      double utility = winners.size() == 1 && winners[0] == i ? 1.0 : -1.0;
      for (auto &path_node : paths[i]) {
        TreeNode &node = *path_node.tree_node;
        if (node.updated) continue;
        node.updated = true;
        int chosen_child = path_node.chosen_child;
        node.total++;
        node.visits[chosen_child]++;
        node.utility[chosen_child] += utility;
        if (node.total > node.regret.size()) {
          for (int j = 0; j < node.regret.size(); j++) {
            double reward = j == chosen_child ? utility : node.utility[j] / node.visits[j];
            node.regret[j] += reward - utility;
          }
        }
      }
    }
  }
  // Choose child weighted by average strategy
  size_t view_hash = get_view_hash(view);
  TreeNode &node = tree[view_hash];
  std::vector<GameAction> actions = view.legal_actions();
  int choice = 0;
  double total_strategy = 0.0;
  for (int i = 0; i < actions.size(); i++) {
    std::cout << node.utility[i] / node.visits[i] << "\t"
              << node.visits[i] << "\t"
              << node.regret[i] << "\t"
              << node.strategy[i] << "\t"
              << json(actions[i]) << std::endl;
  }
  for (int i = 0; i < actions.size(); i++) {
    total_strategy += node.strategy[i];
    if (distribution(gen) < node.strategy[i] / total_strategy) {
      choice = i;
    }
  }
  return actions[choice];
}
