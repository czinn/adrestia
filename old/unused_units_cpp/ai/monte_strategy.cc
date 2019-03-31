#include "monte_strategy.h"
#include <chrono>
#include <cmath>
#include <algorithm>
#include "../action.h"
#include "../game_view.h"
#include "../game_state.h"
#include "../colour.h"

struct TreeNode {
  int total;
  std::vector<int> visits;
  std::vector<int> wins;
};

MonteStrategy::MonteStrategy() : gen(std::chrono::high_resolution_clock::now().time_since_epoch().count()) {}

MonteStrategy::~MonteStrategy() {}

void generate_techs(std::vector<Tech> &possible_techs, Tech t, int total, Colour colour) {
  if (t.red + t.green + t.blue == total) {
    possible_techs.push_back(t);
    return;
  }
  Tech incremented = t;
  incremented.increment(colour);
  generate_techs(possible_techs, incremented, total, colour);
  if (colour < BLUE) {
    generate_techs(possible_techs, t, total, (Colour)(colour + 1));
  }
}

size_t get_view_hash(const GameView &v) {
  // TODO: charles: Choose a better view hash. Right now this is slow and
  // probably bad, but is fast to code and probably works.
  // Hash must, at minimum, include (explicitly or implicitly) coins and tech for current player.
  return std::hash<std::string>{}(json(v.action_log).dump()) ^ std::hash<int>{}(v.view_player.coins) ^ std::hash<std::string>{}(json(v.view_player.tech).dump());
}

Action MonteStrategy::get_action(const GameView &view) {
  // Determine which techs are possible for the opponent.
  std::vector<Tech> visible_tech(view.players.size());
  for (auto &turn : view.action_log) {
    for (int i = 0; i < view.players.size(); i++) {
      for (auto &action : turn[i]) {
        for (auto &unit : action.get_units()) {
          const std::shared_ptr<Tech> tech = view.rules->get_unit_kind(unit).get_tech();
          if (tech) {
            visible_tech[i].max(*tech);
          }
        }
      }
    }
  }
  std::vector<std::vector<Tech>> possible_techs(view.players.size());
  const Tech &my_tech = view.view_player.tech;
  int my_total_tech = my_tech.red + my_tech.green + my_tech.blue;
  for (int i = 0; i < view.players.size(); i++) {
    generate_techs(possible_techs[i], visible_tech[i], my_total_tech, RED);
  }
  
  // Iterations!
  std::map<size_t, TreeNode> tree;
  for (int i = 0; i < 1000; i++) {
    // Choose a tech for each player (determinization).
    // For now, don't do self-determinizations.
    std::vector<Tech> techs;
    for (int i = 0; i < view.players.size(); i++) {
      if (i != view.view_index) {
        techs.push_back(possible_techs[i][gen() % possible_techs[i].size()]);
      } else {
        techs.push_back(view.view_player.tech);
      }
    }
    // Create the determinization.
    GameState g(view, techs);
    GameView v;
    // Expand (i.e. add to the tree) only one node per player per iteration.
    std::vector<bool> expanded(view.players.size());
    std::vector<std::pair<TreeNode*, int>> paths[view.players.size()];
    // While the game isn't finished, look up states in the tree for each player and pick actions.
    while (g.get_winners().size() == 0) {
      for (int i = 0; i < view.players.size(); i++) {
        g.get_view(v, i);
        std::vector<Action> actions = v.legal_actions();
        size_t view_hash = get_view_hash(v);
        // If we haven't expanded this node...
        if (tree.count(view_hash) == 0) {
          // ...and we haven't expanded a node for this player yet this iteration...
          if (!expanded[i]) {
            // ...expand the node.
            expanded[i] = true;
            tree[view_hash] = TreeNode { 0, std::vector<int>(actions.size()), std::vector<int>(actions.size()) };
          } else {
            // Otherwise, choose a random action.
            g.perform_action(i, actions[gen() % actions.size()]);
            continue;
          }
        }
        // Node has been expanded (either previously or just now).
        TreeNode &node = tree[view_hash];
        // If not all children have been tried yet...
        int chosen_child = 0;
        if (node.total < node.visits.size()) {
          // ...choose a random untried child.
          int untried_children = 0;
          for (int j = 0; j < node.visits.size(); j++) {
            if (node.visits[j] == 0) {
              untried_children++;
              if (gen() % untried_children == 0) {
                chosen_child = j;
              }
            }
          }
        } else {
          // Otherwise, choose a node based on UCT
          double best_score = 0.0;
          for (int j = 0; j < node.visits.size(); j++) {
            double score = 1.0 * node.wins[j] / node.visits[j] + 1.41 * sqrt(log(node.total) / node.visits[j]);
            if (score > best_score) {
              best_score = score;
              chosen_child = j;
            }
          }
        }
        g.perform_action(i, actions[chosen_child]);
        paths[i].push_back(std::make_pair(&node, chosen_child));
      }
    }
    // Update node statistics.
    const std::vector<int> &winners = g.get_winners();
    for (int i = 0; i < view.players.size(); i++) {
      bool is_winner = std::find(winners.begin(), winners.end(), i) != winners.end();
      for (auto &[node, chosen_child] : paths[i]) {
        node->total++;
        node->visits[chosen_child]++;
        node->wins[chosen_child] += is_winner;
      }
    }
  }
  // Choose child weighted by number of visits
  size_t view_hash = get_view_hash(view);
  TreeNode &node = tree[view_hash];
  std::vector<Action> actions = view.legal_actions();
  int chosen_visit = gen() % node.total;
  for (int i = 0; i < actions.size(); i++) {
    chosen_visit -= node.visits[i];
    if (chosen_visit <= 0) {
      return actions[i];
    }
  }
  return actions[0]; // Should not be reached
}
