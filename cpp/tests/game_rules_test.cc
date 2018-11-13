#include "../game_rules.h"
#include "../json.h"
#include "catch.hpp"

#include <iostream>
#include <fstream>

using json = nlohmann::json;

static std::string rules_filename = "rules.json";

TEST_CASE("GameRules") {
	GameRules rules(rules_filename);

	SECTION("GameRules round trips to itself") {
		GameRules rules2 = json(rules);
		REQUIRE(rules == rules2);
	}
}
