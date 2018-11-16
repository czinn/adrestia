#include "../game_state.h"
#include "../game_rules.h"
#include "../json.h"
#include "catch.hpp"

#include <fstream>

using json = nlohmann::json;

static std::string rules_filename = "rules.json";

TEST_CASE("GameState") {
	GameRules rules(rules_filename);
	GameState state(rules, (std::vector<std::vector<std::string>>){{"conjuration"}, {"conjuration"}});

	SECTION("players can use tech the turn they get it") {
		REQUIRE(state.is_valid_action(0, {"tech_conjuration", "damage_1"}));
		REQUIRE(!state.is_valid_action(0, {"damage_1", "tech_conjuration", "damage_1"}));
	}

	SECTION("players can't get more mana than the mana cap") {
		for (int i = 0; i < 10; i++) {
			state.simulate({{}, {}});
		}
		REQUIRE(state.players[0].mp <= rules.get_mana_cap());
	}

	SECTION("GameState round trips to itself") {
		state.simulate({{"damage_1"}, {"shield_1"}});
		state.simulate({{"damage_1"}, {"tech_conjuration", "shield_1"}});
		state.simulate({{"damage_1", "damage_1", "damage_1", "damage_1"}, {"shield_1"}});
		GameState state2(rules, json(state));
		REQUIRE(state == state2);
	}

}

TEST_CASE("Conjuration spells") {
	GameRules rules(rules_filename);
	GameState state(rules, (std::vector<std::vector<std::string>>){{"conjuration"}, {"conjuration"}});

	for (int i = 0; i < 7; i++) {
		state.simulate({{"tech_conjuration"}, {"tech_conjuration"}});
	}

	Player &player0 = state.players[0];
	Player &player1 = state.players[1];

	SECTION("damage_1") {
		int hp_before = player1.hp;
		state.simulate({{"damage_1"}, {}});
		REQUIRE(player1.hp == hp_before - 1);
	}

	SECTION("shield_1") {
		int hp_before = player1.hp;
		state.simulate({{"damage_1"}, {"shield_1"}});
		REQUIRE(player1.hp == hp_before);
		state.simulate({{"damage_1"}, {"tech_conjuration", "shield_1"}});
		REQUIRE(player1.hp == hp_before - 1);
		state.simulate({{"damage_1", "damage_1", "damage_1", "damage_1"}, {"shield_1"}});
		REQUIRE(player1.hp == hp_before - 2);
	}

	SECTION("mana_1") {
		player0.mp = 5;
		player0.mp_regen = 0;
		state.simulate({{"mana_1"}, {}});
		REQUIRE(player0.mp == 3);
		REQUIRE(player0.mp_regen == 1);
	}

	SECTION("poison_1") {
		int hp_before = player1.hp;
		state.simulate({{"poison_1"}, {"shield_1"}});
		REQUIRE(player1.hp == hp_before - 1);
		state.simulate({{"poison_1"}, {"shield_1"}});
		REQUIRE(player1.hp == hp_before - 3);
	}

	SECTION("shield_2") {
		int hp_before = player1.hp;
		state.simulate({{"damage_1", "damage_1", "damage_1"}, {"shield_2"}});
		REQUIRE(player1.hp == hp_before);
		state.simulate({{"damage_1", "damage_1"}, {}});
		REQUIRE(player1.hp == hp_before - 1);
		state.simulate({{}, {"shield_2"}});
		state.simulate({{}, {}});
		state.simulate({{"damage_1"}, {}});
		REQUIRE(player1.hp == hp_before - 2);
	}

	SECTION("chain_1") {
		int hp_before = player1.hp;
		state.simulate({{"chain_1"}, {}});
		REQUIRE(player1.hp == hp_before - 3);
		state.simulate({{"chain_1"}, {}});
		REQUIRE(player1.hp == hp_before - 7);
		state.simulate({{"chain_1"}, {}});
		REQUIRE(player1.hp == hp_before - 12);
	}

	SECTION("endgame_1") {
		int hp_before = player1.hp;
		state.simulate({{"endgame_1"}, {"damage_1"}});
		REQUIRE(player1.hp == hp_before - 4);
		state.simulate({{"endgame_1"}, {"damage_1", "damage_1", "tech_conjuration", "shield_1"}});
		REQUIRE(player1.hp == hp_before - 11);
	}
}
