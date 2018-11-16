#include "../game_state.h"
#include "../game_rules.h"
#include "../json.h"
#include "catch.hpp"

#include <fstream>
#include <iostream>

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

TEST_CASE("Mana drains") {
	GameRules rules(rules_filename);
	GameState state(rules,
			std::vector<std::vector<std::string>>
			{{"conjuration", "testing"}, {"conjuration", "testing"}});

	auto &p0 = state.players[0];
	auto &p1 = state.players[1];
	const int p0_starting_hp = p0.hp;
	const int p1_starting_hp = p1.hp;
	p0.mp = 10;
	p0.mp_regen = 3;
	p1.mp = 10;
	p1.mp_regen = 3;

	state.simulate({{
		"tech_conjuration",
		"damage_1", "damage_1", "damage_1", "damage_1", "damage_1",
		"damage_1", "damage_1", "damage_1", "damage_1", "damage_1",
	}, {
		"tech_conjuration",
		"burn_mana_1", "burn_mana_1", "burn_mana_1", "burn_mana_1", "burn_mana_1",
		"damage_1", "damage_1"
	}});

	SECTION("mana cannot go below 0") {
		REQUIRE(p0.mp >= 0);
	}

	SECTION("mana regen happens after burns") {
		REQUIRE(p0.mp == p0.mp_regen);
	}

	SECTION("correct number of fizzles") {
		// p0.mp   p1 health loss
		// 0       0
		// 1-4     1
		// 5-8     2
		// 9-12    3
		REQUIRE(p1_starting_hp - p1.hp == 3);
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

TEST_CASE("Regulation spells") {
	GameRules rules(rules_filename);
	GameState state(rules, (std::vector<std::vector<std::string>>){{"regulation", "conjuration"}, {"conjuration", "regulation"}});

	for (int i = 0; i < 7; i++) {
		state.simulate({{"regulation_tech"}, {"tech_conjuration"}});
	}

	Player &player0 = state.players[0];
	Player &player1 = state.players[1];

	SECTION("regulation_cancel_1") {
		int hp0 = player0.hp;
		int hp1 = player1.hp;
		REQUIRE(state.simulate({{"regulation_cancel_1", "regulation_cancel_1", "tech_conjuration", "damage_1"}, {"shield_1", "damage_1"}}));
		REQUIRE(player0.hp == hp0 - 1);
		REQUIRE(player1.hp == hp1 - 1);
	}

	SECTION("regulation_cancel_2") {
		int hp0 = player0.hp;
		int hp1 = player1.hp;
		state.simulate({{"regulation_cancel_2", "regulation_cancel_2", "tech_conjuration", "damage_1"}, {"shield_1", "damage_1"}});
		REQUIRE(player0.hp == hp0);
		REQUIRE(player1.hp == hp1);
	}

	// TODO: charles: Test some of the other non-trivial spells in this book.
}
