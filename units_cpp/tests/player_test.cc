#include "../player.h"
#include "../tech.h"
#include "catch.hpp"

#include <iostream>

TEST_CASE("Player") {
  GameRules rules("rules.json");
  Player p(rules);

  REQUIRE(p.units.size() == rules.get_starting_units().size());
  REQUIRE(p.alive);
  REQUIRE(p.tech == Tech());
  REQUIRE(p.coins == 0);

  SECTION("building units") {
    p.build_unit(rules.get_unit_kind("turret"));
    p.build_unit(rules.get_unit_kind("grunt"));
    REQUIRE(p.units.size() == rules.get_starting_units().size() + 2);
  }

  SECTION("begin_turn increases coins by total font") {
    p.begin_turn();
    REQUIRE(p.coins == 5);
    p.build_unit(rules.get_unit_kind("worker"));
    p.begin_turn();
    REQUIRE(p.coins == 11);
  }
}
