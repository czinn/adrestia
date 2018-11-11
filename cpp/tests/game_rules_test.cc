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
		// Currently fails because GameRules does not preserve the order of spells/books.
		// XTODO: charles: Figure out how to compare with considering order.
		// jim: Is it important that we are able to get the original JSON after
		// converting to GameRules? I suspect that the GameRules -> json ->
		// GameRules roundtrip is the important one. (We do have to define
		// operator== for everything now but that would probably be a good idea
		// anyway?)
		// jim: So defining operator=='s was more of a challenge than I thought
		// (you'd think that C++ defines a default just like it defines a default
		// copy constructor...) but this is done now. If we want an isomorphism we
		// can define another test.
		json j = json(rules);
		GameRules rules2;
		from_json(j, rules2);
		REQUIRE(rules == rules2);
	}
}
