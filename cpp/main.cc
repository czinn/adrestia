#include <iostream>
#include "json.h"
#include "game_rules.h"
#include "player.h"
#include "game_state.h"

using json = nlohmann::json;

void print_game(const GameState &state) {
	std::cout << "---" << std::endl;
	std::cout << "Turn " << state.turn_number() << std::endl;
	for (size_t player_id = 0; player_id < state.players.size(); player_id++) {
		const auto &player = state.players[player_id];
		std::cout << "Player " << player_id;
		std::cout << " (hp: " << player.hp << "/" << player.max_hp << ")";
		std::cout << " (mp: " << player.mp << " (+" << player.mp_regen << "))";
		std::cout << " (books:";
		for (size_t i = 0; i < player.books.size(); i++) {
			const auto tech = player.tech[i];
			const auto *book = player.books[i];
			std::cout << " (" << book->get_name() << ": " << tech << ")";
		}
		std::cout << ")";
		std::cout << std::endl;
	}
}

int main() {
	GameRules rules("rules.json");

	GameState state(rules, {{"conjuration"}, {"conjuration"}});
	print_game(state);

	const std::vector<std::vector<GameAction>> turns {
		{{"tech_conjuration"}, {"tech_conjuration"}},
		{{"tech_conjuration", "damage_1", "damage_1", "damage_1", "damage_1", "damage_1"}, {"tech_conjuration"}},
		{{"tech_conjuration", "damage_1", "damage_1", "damage_1", "damage_1", "damage_1"}, {"tech_conjuration"}},
		{{"tech_conjuration", "damage_1", "damage_1", "damage_1", "damage_1", "damage_1"}, {"tech_conjuration"}},
		{{"tech_conjuration", "damage_1", "damage_1", "damage_1", "damage_1", "damage_1"}, {"tech_conjuration"}},
		{{"tech_conjuration", "damage_1", "damage_1", "damage_1", "damage_1", "damage_1"}, {"tech_conjuration"}},
		{{"tech_conjuration", "damage_1", "damage_1", "damage_1", "damage_1", "damage_1"}, {"tech_conjuration"}},
		{{"tech_conjuration", "damage_1", "damage_1", "damage_1", "damage_1", "damage_1"}, {"tech_conjuration"}},
	};
	for (const auto &turn : turns) {
		state.simulate(turn);
		print_game(state);
		if (state.winners().size() > 0) {
			std::cout << "---" << std::endl;
			std::cout << "Winners:";
			for (const auto &winner : state.winners()) {
				std::cout << " " << winner;
			}
			std::cout << std::endl;
			break;
		}
	}
	return 0;
}
