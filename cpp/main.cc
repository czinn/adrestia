#include <iostream>
#include "json.h"
#include "game_rules.h"
#include "player.h"
#include "game_state.h"

using json = nlohmann::json;

int main() {
	GameRules rules("rules.json");
	GameState state(rules, {{"conjuration"}, {"conjuration"}});
	std::cout << json(state) << std::endl;
	return 0;
}
