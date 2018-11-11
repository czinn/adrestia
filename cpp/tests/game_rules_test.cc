#include "../game_rules.h"
#include "../json.h"
#include "catch.hpp"

#include <iostream>

using json = nlohmann::json;

TEST_CASE("GameRules") {
	GameRules rules("rules.json");
	std::cout << json(rules) << std::endl;
}
