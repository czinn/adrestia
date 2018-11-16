#include "game_view.h"

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
				if(player_id == view_player_id || !rules.get_spell(it).is_tech_spell()) {
					history[i][player_id].push_back(it);
				} else {
					history[i][player_id].push_back(tech_spell_id);
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

//------------------------------------------------------------------------------
// SERIALIZATION
//------------------------------------------------------------------------------
void to_json(json &j, const GameView &view) {
	j["players"] = view.players;
	j["history"] = view.history;
	j["view_player_id"] = view.view_player_id;
}
