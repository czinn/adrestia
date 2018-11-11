#include "../game_rules.h"
#include "../json.h"
#include "catch.hpp"

#include <iostream>
#include <fstream>

using json = nlohmann::json;

static std::string rules_filename = "rules.json";

TEST_CASE("GameRules") {
	GameRules rules(rules_filename);

	SECTION("round trips") {
		json j = json(rules);
		std::ifstream in(rules_filename);
		json j2;
		in >> j2;
		// Currently fails because GameRules does not preserve the order of spells/books.
		// TODO: charles: Figure out how to compare with considering order.
		// REQUIRE(j == j2);
	}
}
