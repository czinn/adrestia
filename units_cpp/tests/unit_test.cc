#include "../unit.h"
#include "../json.h"
#include "catch.hpp"

using json = nlohmann::json;

TEST_CASE("Unit") {
  UnitKind kind = R"(
    {
      "id": "super_turret",
      "name": "Super Turret",
      "colour": "GREEN",
      "health": 4,
      "shields": 2,
      "width": 3,
      "build_time": 1,
      "attack": [3, 1],
      "cost": 6,
      "tech": {"green": 3, "blue": 3},
      "image": "superturret.png",
      "tiles": [0, 0, 0, 1, 0, 2],
      "label": "S"
    })"_json;

  Unit unit(kind);

  SECTION("initial values correct") {
    REQUIRE(&unit.kind == &kind);
    REQUIRE(unit.health == kind.get_health());
    REQUIRE(unit.shields == kind.get_shields());
    REQUIRE(unit.build_time == kind.get_build_time());
  }

  SECTION("copied values correct") {
    unit.health -= 1;
    unit.shields -= 1;
    unit.build_time -= 1;
    Unit copy(unit);
    REQUIRE(&copy.kind == &kind);
    REQUIRE(copy.health == kind.get_health() - 1);
    REQUIRE(copy.shields == kind.get_shields() - 1);
    REQUIRE(copy.build_time == kind.get_build_time() - 1);
  }
}
