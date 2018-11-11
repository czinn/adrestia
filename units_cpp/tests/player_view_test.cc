#include "../player.h"
#include "../player_view.h"
#include "catch.hpp"

TEST_CASE("PlayerView") {
  GameRules rules("rules.json");
  Player p(rules);
  PlayerView v(p);

  SECTION("view's components are equal to player's") {
    REQUIRE(p.units.size() == v.units.size());
    for (auto it = p.units.begin(); it != p.units.end(); it++) {
      REQUIRE(&it->second.kind == &v.units.at(it->first).kind);
      REQUIRE(it->second.health == v.units.at(it->first).health);
      REQUIRE(it->second.shields == v.units.at(it->first).shields);
      REQUIRE(it->second.build_time == v.units.at(it->first).build_time);
    }
    REQUIRE(p.alive == v.alive);
  }

  SECTION("modifying view does not modify player") {
    int original_health = v.units.at(0).health;
    v.units.at(0).health--;
    REQUIRE(p.units.at(0).health == original_health);
  }

  SECTION("modifying player does not modify view") {
    int original_health = p.units.at(0).health;
    p.units.at(0).health--;
    REQUIRE(v.units.at(0).health == original_health);
  }
}
