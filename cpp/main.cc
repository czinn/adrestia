#include <iostream>
#include "json.h"
#include "game_rules.h"
#include "player.h"
#include "game_state.h"

using json = nlohmann::json;

int main() {
	GameRules rules("rules.json");

	GameState state(rules, (std::vector<std::vector<std::string>>){{"conjuration"}, {"conjuration"}});
	std::cout << state;

	const std::vector<std::vector<GameAction>> turns {
		{{"tech_conjuration", "damage_1", "damage_1", "damage_1", "damage_1", "damage_1"}, {"tech_conjuration"}},
		{{"tech_conjuration", "damage_1", "damage_1", "damage_1", "damage_1", "damage_1"}, {"tech_conjuration", "poison_1", "poison_1"}},
		{{"tech_conjuration", "damage_1", "damage_1", "damage_1", "damage_1", "damage_1"}, {"tech_conjuration", "poison_1"}},
		{{"tech_conjuration", "damage_1", "damage_1", "damage_1", "damage_1", "damage_1"}, {"tech_conjuration", "poison_1"}},
		{{"tech_conjuration", "damage_1", "damage_1", "damage_1", "damage_1"}, {"tech_conjuration", "poison_1"}},
		{{"tech_conjuration"}, {"tech_conjuration", "poison_1"}},
		{{"tech_conjuration"}, {"tech_conjuration", "poison_1"}},
		{{"tech_conjuration"}, {"tech_conjuration", "poison_1"}},
		{{"tech_conjuration"}, {"tech_conjuration", "poison_1"}},
	};
	for (const auto &turn : turns) {
		state.simulate(turn);
		std::cout << state;
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
