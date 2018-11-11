#include "../game_state.h"
#include "../game_rules.h"
#include "../colour.h"
#include "../battle.h"
#include "../json.h"
#include "catch.hpp"

using json = nlohmann::json;

TEST_CASE("GameState") {
  GameRules rules("rules.json");
  GameState game(rules, 2);
  Battle::set_seed(12345678);

  REQUIRE(game.perform_action(0, Action(RED)));
  REQUIRE(!game.perform_action(0, Action(BLUE)));
  REQUIRE(!game.perform_action(1, Action(BLACK)));
  REQUIRE(game.perform_action(1, Action(GREEN)));
  REQUIRE(!game.perform_action(0, Action(std::vector<std::string>({ "grunt", "turret" }))));
  REQUIRE(game.perform_action(0, Action(std::vector<std::string>({ "grunt", "grunt" }))));
  REQUIRE(game.perform_action(1, Action(std::vector<std::string>({ "turret", "turret" }))));

  SECTION("json representation") {
    json j = json(game);
    REQUIRE(j == R"(
    {"action_log":[[[{"colour":"RED","type":"CHOOSE_TECH"},{"type":"BUILD_UNITS","units":["grunt","grunt"]}],[{"colour":"GREEN","type":"CHOOSE_TECH"},{"type":"BUILD_UNITS","units":["turret","turret"]}]],[[],[]]],"battles":[{"attacks":[[0,0,1,7,2],[0,6,1,6,1],[0,7,1,5,1],[1,0,0,4,2],[1,6,0,1,1],[1,7,0,0,1]],"players":[{"alive":true,"units":{"0":{"build_time":0,"health":5,"kind":"general","shields":0},"1":{"build_time":0,"health":1,"kind":"worker","shields":2},"2":{"build_time":0,"health":1,"kind":"worker","shields":2},"3":{"build_time":0,"health":1,"kind":"worker","shields":2},"4":{"build_time":0,"health":1,"kind":"worker","shields":2},"5":{"build_time":0,"health":1,"kind":"worker","shields":2},"6":{"build_time":0,"health":1,"kind":"grunt","shields":0},"7":{"build_time":0,"health":1,"kind":"grunt","shields":0}}},{"alive":true,"units":{"0":{"build_time":0,"health":5,"kind":"general","shields":0},"1":{"build_time":0,"health":1,"kind":"worker","shields":2},"2":{"build_time":0,"health":1,"kind":"worker","shields":2},"3":{"build_time":0,"health":1,"kind":"worker","shields":2},"4":{"build_time":0,"health":1,"kind":"worker","shields":2},"5":{"build_time":0,"health":1,"kind":"worker","shields":2},"6":{"build_time":0,"health":1,"kind":"turret","shields":0},"7":{"build_time":0,"health":1,"kind":"turret","shields":0}}}]}],"players":[{"alive":true,"coins":6,"next_unit":8,"tech":{"red":1},"units":{"0":{"build_time":0,"health":4,"kind":"general","shields":0},"1":{"build_time":0,"health":1,"kind":"worker","shields":2},"2":{"build_time":0,"health":1,"kind":"worker","shields":2},"3":{"build_time":0,"health":1,"kind":"worker","shields":2},"4":{"build_time":0,"health":1,"kind":"worker","shields":2},"5":{"build_time":0,"health":1,"kind":"worker","shields":2},"6":{"build_time":0,"health":1,"kind":"grunt","shields":0},"7":{"build_time":0,"health":1,"kind":"grunt","shields":0}}},{"alive":true,"coins":6,"next_unit":8,"tech":{"green":1},"units":{"0":{"build_time":0,"health":5,"kind":"general","shields":0},"1":{"build_time":0,"health":1,"kind":"worker","shields":2},"2":{"build_time":0,"health":1,"kind":"worker","shields":2},"3":{"build_time":0,"health":1,"kind":"worker","shields":2},"4":{"build_time":0,"health":1,"kind":"worker","shields":2},"5":{"build_time":0,"health":1,"kind":"worker","shields":2}}}],"players_ready":0,"turn":2}
    )"_json);
    REQUIRE(json(GameState(rules, j)) == j);
  }

  SECTION("game terminates") {
    while (game.get_winners().size() == 0) {
      REQUIRE(game.perform_action(0, Action(BLUE)));
      REQUIRE(game.perform_action(1, Action(BLUE)));
      REQUIRE(game.perform_action(0, Action(std::vector<std::string>({}))));
      REQUIRE(game.perform_action(1, Action(std::vector<std::string>({}))));
    }

    REQUIRE(game.get_action_log().size() == game.get_turn());
    REQUIRE(game.get_action_log()[0][0][0] == Action(RED));
    REQUIRE(game.get_action_log()[0][0][1] == Action(std::vector<std::string>({"grunt", "grunt"})));
  }
}
