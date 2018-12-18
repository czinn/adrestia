#include "cfr_strategy.h"
#include "monte_strategy.h"
#include <chrono>
#include <cmath>
#include <algorithm>
#include <vector>
#include <set>
#include <iostream>
#include <unordered_map>
#include "game_action.h"
#include "game_view.h"
#include "game_state.h"

struct Node {
	double regret;
	double strategy;
};

std::unordered_map<size_t, std::vector<GameAction>> CfrStrategy::action_cache;

const std::vector<GameAction> *CfrStrategy::get_view_actions(const GameView &view) {
	const Player &p = view.players[view.view_player_id];
	size_t h = std::hash<int>{}(p.mp);
	for (size_t i = 0; i < p.tech.size(); i++) {
		h += std::hash<std::string>{}(p.books[i]->get_id()) * p.tech[i];
	}
	auto it = action_cache.find(h);
	if (it != action_cache.end()) {
		return &it->second;
	} else {
		action_cache.emplace(h, view.legal_actions());
		return &action_cache[h];
	}
}

std::vector<double> cfr_state_vector(const GameState &g) {
	std::vector<double> r;
	for (size_t i = 0; i < 2; i++) {
		const Player &p = g.players[i];
		r.push_back(1.0 * std::max(0, p.hp) / p.max_hp);
		r.push_back(1.0 * p.mp / g.rules.get_mana_cap());
		r.push_back(std::min(1.0, 1.0 * p.mp_regen / g.rules.get_mana_cap()));
		for (const auto &[book_id, book] : g.rules.get_books()) {
			size_t book_index = p.find_book_idx(book_id);
			r.push_back(book_index != size_t(-1) ? 1.0 : 0.0);
			for (size_t tech = 1; tech <= 4; tech++) {
				r.push_back(p.tech[book_index] >= tech ? 1.0 : 0.0);
			}
		}
	}
	return r;
}

std::uniform_real_distribution<double> dis(0.0, 1.0);

CfrStrategy::CfrStrategy()
		: gen(std::chrono::high_resolution_clock::now().time_since_epoch().count())
		, weights({
				25 * 5, 10 * 1, 10 * 2,
				0, 5, 5, 5, 5,
				0, 5, 5, 5, 5,
				25 * -5, 10 * -1, 10 * -2,
				0, -5, -5, -5, -5,
				0, -5, -5, -5, -5})
		, model(nullptr) {}

CfrStrategy::CfrStrategy(std::vector<double> weights)
		: gen(std::chrono::high_resolution_clock::now().time_since_epoch().count())
		, weights(weights)
		, model(nullptr) {}

CfrStrategy::CfrStrategy(fdeep::model *model)
		: gen(std::chrono::high_resolution_clock::now().time_since_epoch().count())
		, model(model) {}

CfrStrategy::~CfrStrategy() {}

size_t get_action_hash(size_t player, const GameAction &a) {
	size_t h = 0;
	for (const auto &s : a) {
		h = (h << 4) ^ std::hash<std::string>{}(s);
	}
	return h ^ std::hash<size_t>{}(player);
}

double CfrStrategy::score_game_state(const GameState &g) const {
	std::vector<double> vec = cfr_state_vector(g);
	if (model == nullptr) {
		double score = 0;
		for (size_t i = 0; i < weights.size(); i++) {
			score += weights[i] * vec[i];
		}
		return score;
	} else {
		std::vector<float> float_vec;
		for (double d : vec) {
			float_vec.push_back((float)d);
		}
		const fdeep::shared_float_vec sv(fplus::make_shared_ref<fdeep::float_vec>(float_vec));
		const fdeep::tensor5 t(fdeep::shape5(1, 1, 1, 1, vec.size()), sv);
		return model->predict({t})[0].get(0, 0, 0, 0, 0);
	}
}

double CfrStrategy::score_action_pair(
		const GameState &g,
		const GameAction &a0,
		const GameAction &a1,
		std::unordered_map<size_t, double> &score_map) const {
	size_t h = get_action_hash(0, a0) ^ get_action_hash(1, a1);
	auto it = score_map.find(h);
	if (it != score_map.end()) {
		return it->second;
	} else {
		GameState gp(g);
		assert(gp.simulate({a0, a1}));
		double score = score_game_state(gp);
		score_map.emplace(h, score);
		return score;
	}
}

GameAction CfrStrategy::get_action(const GameView &view) {
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
	std::unordered_map<size_t, Node> regret_map;
	std::unordered_map<size_t, double> score_map;
	for (int i = 0; i < 100; i++) {
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
		// Get the actions available for each player, memoizing them
		std::vector<const std::vector<GameAction>*> actions;
		for (size_t i = 0; i < 2; i++) {
			// TODO: Optimize by not converting to a game view first
			GameView v(g, i);
			actions.push_back(CfrStrategy::get_view_actions(v));
		}
		// Choose an action for each player, based on regret.
		std::vector<GameAction> chosen_actions;
		double total_regret[2] = {0};
		double total_count[2] = {0};
		bool made_choice[2] = {0};
		for (size_t i = 0; i < 2; i++) {
			GameAction chosen_action;
			GameAction random_action;
			for (const auto &a : *actions[i]) {
				size_t h = get_action_hash(i, a);
				double regret = 0.0;
				auto it = regret_map.find(h);
				if (it != regret_map.end()) {
					regret = it->second.regret;
				}
				total_regret[i] += std::max(0.0, regret);
				if (total_regret[i] > 0 && dis(gen) <= regret / total_regret[i]) {
					made_choice[i] = true;
					chosen_action = a;
				}
				total_count[i] += 1.0;
				if (dis(gen) <= 1.0 / total_count[i]) {
					random_action = a;
				}
			}
			chosen_actions.push_back(made_choice[i] ? chosen_action : random_action);
		}
		// Update strategies.
		for (size_t i = 0; i < 2; i++) {
			for (const auto &a : *actions[i]) {
				size_t h = get_action_hash(i, a);
				Node &n = regret_map[h];
				n.strategy +=
					made_choice[i] ?
					std::max(0.0, n.regret) / total_regret[i] :
					1.0 / actions[i]->size();
			}
		}
		// Determine the actual result of the game.
		double actual_score = score_action_pair(g, chosen_actions[0], chosen_actions[1], score_map);
		// Update regrets.
		for (size_t i = 0; i < 2; i++) {
			for (const auto &a : *actions[i]) {
				size_t h = get_action_hash(i, a);
				Node &n = regret_map[h];
				// How much do we regret not doing this action?
				double score =
					i == 0 ?
					score_action_pair(g, a, chosen_actions[1], score_map) :
					score_action_pair(g, chosen_actions[0], a, score_map);
				double regret_delta = i == 0 ? (score - actual_score) : (actual_score - score);
				n.regret += regret_delta;
			}
		}
	}
	// Choose an action based on strategy.
	const std::vector<GameAction> &actions = *get_view_actions(view);
	GameAction chosen_action;
	double total_strategy = 0.0;
	for (const auto &a : actions) {
		size_t h = get_action_hash(view.view_player_id, a);
		double strategy = std::max(0.0, regret_map[h].strategy);
		total_strategy += strategy;
		if (dis(gen) <= strategy / total_strategy) {
			chosen_action = a;
		}
	}
	return chosen_action;
}
