#include "game_view.h"
#include "spell.h"

//------------------------------------------------------------------------------
// C++ SEMANTICS
//------------------------------------------------------------------------------
GameView::GameView(const GameState &state, size_t view_player_id)
	: players(state.players)
	, view_player_id(view_player_id)
	, rules(state.rules) {
	for (size_t i = 0; i < state.history.size(); i++) {
		history.push_back(std::vector<GameAction>());
		for (size_t player_id = 0; player_id < state.players.size(); player_id++) {
			history[i].push_back(GameAction());
			for (const auto &it : state.history[i][player_id]) {
				if(player_id == view_player_id ||
						(it != TECH_SPELL_ID && !rules.get_spell(it).is_tech_spell())) {
					history[i][player_id].push_back(it);
				} else {
					history[i][player_id].push_back(TECH_SPELL_ID);
				}
			}
		}
	}
	for (size_t player_id = 0; player_id < state.players.size(); player_id++) {
		if (player_id != view_player_id) {
			players[player_id].books.clear();
			players[player_id].tech.clear();
		}
	}
}

GameView::GameView(const GameRules &rules, const json &j)
	: view_player_id(j.at("view_player_id"))
	, rules(rules) {
	for (const auto &it : j.at("players")) {
		players.push_back(Player(rules, it));
	}
	for (const auto &it : j.at("history")) {
		history.push_back(it);
	}
}

//------------------------------------------------------------------------------
// BUSINESS LOGIC
//------------------------------------------------------------------------------
int GameView::turn_number() const {
	return history.size() + 1;
}

std::vector<size_t> GameView::winners() const {
	std::vector<size_t> alive;
	for (size_t i = 0; i < players.size(); i++) {
		if (players[i].hp > 0) alive.push_back(i);
	}
	if (alive.size() == 1) return alive;
	if (alive.size() == 0) {
		for (size_t i = 0; i < players.size(); i++) {
			alive.push_back(i);
		}
		return alive;
	}
	return {};
}

void GameView::generate_actions(
		std::vector<GameAction> &actions, // Accumulator
		GameAction &current, // Current proposed action
		int mana, // Remaining mana
		int turn_tech // The tech built this turn, or -1 for none yet
		) const {
	const Player &p = players[view_player_id];
	actions.push_back(current);
	// If we've already cast the max number of spells this turn, we're done.
	if (current.size() >= rules.get_spell_cap()) {
		return;
	}
	// If we haven't built a tech, we can build a tech.
	if (turn_tech == -1) {
		for (int i = 0; i < p.books.size(); i++) {
			current.push_back(p.books[i]->get_id() + "_tech");
			generate_actions(actions, current, mana, i);
			current.pop_back();
		}
	}
	// If we're out of mana, we can't cast anything.
	if (mana == 0) {
		return;
	}
	// Otherwise, look through all our spells and figure out which ones we can cast.
	int level = p.level() + (turn_tech != -1 ? 1 : 0);
	for (int i = 0; i < p.books.size(); i++) {
		int tech = p.tech[i] + (turn_tech == i ? 1 : 0);
		for (const auto &spell_id : p.books[i]->get_spells()) {
			const Spell &spell = rules.get_spell(spell_id);
			if (!spell.is_tech_spell() && tech >= spell.get_tech() &&
					level >= spell.get_level() && mana >= spell.get_cost()) {
				current.push_back(spell_id);
				generate_actions(actions, current, mana - spell.get_cost(), turn_tech);
				current.pop_back();
			}
		}
	}
}

std::vector<GameAction> GameView::legal_actions() const {
	std::vector<GameAction> actions;
	GameAction current;
	const Player &p = players[view_player_id];
	generate_actions(actions, current, p.mp, -1);
	return actions;
}

// Note, this is not uniformly random; it is slightly biased towards casting
// less than the maximum number of spells.
GameAction GameView::random_action(std::mt19937 &gen) const {
	GameAction a;
	int turn_tech = -1;
	const Player &p = players[view_player_id];
	int mana = p.mp;
	int total_spells = 0;
	while (mana > 0 && total_spells < rules.get_spell_cap()) {
		int level = p.level() + (turn_tech != -1 ? 1 : 0);
		std::string chosen_spell = "";
		int spell_count = 1;
		for (int i = 0; i < p.books.size(); i++) {
			int tech = p.tech[i] + (turn_tech == i ? 1 : 0);
			for (const auto &spell_id : p.books[i]->get_spells()) {
				const Spell &spell = rules.get_spell(spell_id);
				if ((spell.is_tech_spell() && turn_tech == -1) ||
						(!spell.is_tech_spell() && tech >= spell.get_tech() &&
								level >= spell.get_level() && mana >= spell.get_cost())) {
					spell_count++;
					if (gen() % spell_count == 0) {
						chosen_spell = spell_id;
					}
				}
			}
		}
		if (chosen_spell == "") break;
		a.push_back(chosen_spell);
		const Spell &spell = rules.get_spell(chosen_spell);
		mana -= spell.get_cost();
		if (spell.is_tech_spell()) {
			turn_tech = p.find_book_idx(spell.get_book());
		}
		total_spells++;
	}
	// If tech_turn is still -1 and we have space, choose a random new tech
	if (turn_tech == -1 && total_spells < rules.get_spell_cap()) {
		a.push_back(p.books[gen() % p.books.size()]->get_id() + "_tech");
	}
	return a;
}

//------------------------------------------------------------------------------
// SERIALIZATION
//------------------------------------------------------------------------------
void to_json(json &j, const GameView &view) {
	j["players"] = view.players;
	j["history"] = view.history;
	j["view_player_id"] = view.view_player_id;
}
