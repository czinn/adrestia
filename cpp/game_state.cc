#include "game_state.h"

//------------------------------------------------------------------------------
// C++ SEMANTICS
//------------------------------------------------------------------------------
GameState::GameState(
	const GameRules &rules,
	const std::vector<std::vector<std::string>> &player_books)
	: rules(rules) {
	for (const auto &p : player_books) {
		players.emplace_back(rules, p);
	}
}

bool GameState::operator==(const GameState &other) const {
	return this == &other;
}

//------------------------------------------------------------------------------
// SERIALIZATION
//------------------------------------------------------------------------------
void to_json(json &j, const GameState &state) {
	for (auto p : state.players) {
		j["players"].push_back(p);
	}
}
