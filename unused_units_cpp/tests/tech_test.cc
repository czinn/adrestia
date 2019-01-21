#include "../tech.h"
#include "../colour.h"
#include "../json.h"
#include "catch.hpp"

using json = nlohmann::json;

TEST_CASE("Tech") {
  Tech r1(1,2,3);
  Tech r2(2,3,1);
  Tech r3(1,2,4);

  SECTION("default value") {
    REQUIRE(Tech() == Tech(0, 0, 0));
  }

  SECTION("equality") {
    REQUIRE(r1 == r1);
    REQUIRE(!(r1 == r2));
  }
  
  SECTION("addition and includes") {
    REQUIRE((r1 + r2 == Tech(3, 5, 4)));
    REQUIRE(r1 <= r1);
    REQUIRE(!(r1 <= r2));
    REQUIRE(!(r2 <= r1));
    REQUIRE(r1 <= r3);
    REQUIRE(!(r3 <= r1));
  }

  SECTION("increment") {
    Tech r(0, 0, 1);
    r.increment(RED);
    REQUIRE(r == Tech(1, 0, 1));
    r.increment(GREEN);
    r.increment(BLACK);
    REQUIRE(r == Tech(1, 1, 1));
  }

  SECTION("json conversion") {
    REQUIRE(json(r1).get<Tech>() == r1);
    REQUIRE(json(r1) == R"({"blue":3,"green":2,"red":1})"_json);
  }
}
