#include "../game_state.h"
#include "../game_rules.h"
#include "../json.h"
#include "catch.hpp"

#include <iostream>
#include <fstream>

using json = nlohmann::json;

static std::string rules_filename = "rules.json";

TEST_CASE("GameState") {
	GameRules rules(rules_filename);
	GameState state(rules, {{"conjuration"}, {"conjuration"}});

	SECTION("players can use tech the turn they get it") {
		REQUIRE(state.is_valid_action(0, {"tech_conjuration", "damage_1"}));
		REQUIRE(!state.is_valid_action(0, {"damage_1", "tech_conjuration", "damage_1"}));
	}

	SECTION("players can't get more mana than the mana cap") {
		for (int i = 0; i < 10; i++) {
			state.simulate({{}, {}});
		}
		REQUIRE(state.get_players()[0].mp <= rules.get_mana_cap());
	}
}
