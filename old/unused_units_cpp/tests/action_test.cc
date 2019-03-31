#include "../tech.h"
#include "../action.h"
#include "../game_rules.h"
#include "../json.h"
#include "catch.hpp"

using json = nlohmann::json;

TEST_CASE("Action") {
  SECTION("choose tech action") {
    Action a(GREEN);
    REQUIRE(a.get_type() == CHOOSE_TECH);
    REQUIRE(a.get_colour() == GREEN);
    REQUIRE(json(a) == R"({"type":"CHOOSE_TECH","colour":"GREEN"})"_json);
    REQUIRE(json(a).get<Action>() == a);
  }

  SECTION("build units action") {
    GameRules rules("rules.json");
    Action a(std::vector<std::string>({"turret"}));
    REQUIRE(a.get_type() == BUILD_UNITS);
    REQUIRE(a.get_units().size() == 1);
    REQUIRE(a.get_units()[0] == "turret");
    REQUIRE(json(a) == R"({"type":"BUILD_UNITS","units":["turret"]})"_json);
    REQUIRE(json(a).get<Action>() == a);
  }
}
