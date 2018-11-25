#include "monte_strategy.h"
#include <chrono>
#include <cmath>
#include <algorithm>
#include <vector>
#include <set>
#include <iostream>
#include "game_action.h"
#include "game_view.h"
#include "game_state.h"

struct TreeNode {
	int total;
	std::vector<GameAction> actions;
	std::vector<int> visits;
	std::vector<int> wins;
};

MonteStrategy::MonteStrategy() : gen(std::chrono::high_resolution_clock::now().time_since_epoch().count()) {}

MonteStrategy::~MonteStrategy() {}

size_t get_view_hash(const GameView &v) {
	// TODO: charles: Choose a better view hash. Right now this is slow and
	// probably bad, but is fast to code and probably works.
	// Hash must, at minimum, include (explicitly or implicitly) mana and tech
	// for current player.
	return std::hash<std::string>{}(json(v.view_player_id).dump() + json(v.history).dump());
}

// Returns a random set containing k integers in the range [0, n-1]
std::set<int> choose(std::mt19937 &gen, int n, int k) {
	std::set<int> r;
	while (r.size() < k) {
		r.insert(gen() % n);
	}
	return r;
}

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
				if (spell_id == tech_spell_id) {
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
	for (int i = 0; i < 100000; i++) {
		if (i % 10000 == 0) {
			std::cout << i << std::endl;
		}
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
		std::vector<bool> expanded(view.players.size());
		std::vector<std::pair<TreeNode*, int>> paths[view.players.size()];
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
					if (!expanded[i] && turn_number == 0) {
						// ...expand the node.
						expanded[i] = true;
						std::vector<GameAction> actions = v.sane_actions();
						std::cout << "Generated " << actions.size() << " actions" << std::endl;
						tree[view_hash] = TreeNode { 0, actions, std::vector<int>(actions.size()), std::vector<int>(actions.size()) };
					} else {
						// Otherwise, choose a random action.
						the_turn.push_back(v.random_action(gen));
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
				the_turn.push_back(node.actions[chosen_child]);
				paths[i].push_back(std::make_pair(&node, chosen_child));
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
	std::vector<GameAction> actions = view.legal_actions();
	int chosen_visit = gen() % node.total;
	for (int i = 0; i < actions.size(); i++) {
		chosen_visit -= node.visits[i];
		if (chosen_visit <= 0) {
			return actions[i];
		}
	}
	return actions[0]; // Should not be reached
}
