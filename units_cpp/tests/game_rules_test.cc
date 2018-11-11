#include "../unit_kind.h"
#include "../game_rules.h"
#include "../json.h"
#include "catch.hpp"
#include <fstream>

using json = nlohmann::json;

TEST_CASE("GameRules") {
  GameRules rules("rules.json");

  SECTION("get unit kind") {
    const UnitKind &kind = rules.get_unit_kind("general");
    REQUIRE(kind.get_id() == "general");
  }

  SECTION("starting resources and units") {
    REQUIRE(rules.get_unit_cap() == 50);
    REQUIRE(rules.get_starting_units().size() == 6);
  }
}
