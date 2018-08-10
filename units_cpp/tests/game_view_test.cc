#include "../game_view.h"
#include "../game_state.h"
#include "../game_rules.h"
#include "../colour.h"
#include "../battle.h"
#include "../json.h"
#include "catch.hpp"

using json = nlohmann::json;

TEST_CASE("GameView") {
  GameRules rules("rules.json");
  GameState game(rules, 2);
  GameView view;
  game.get_view(view, 0);
  REQUIRE(view.legal_actions().size() == 3);
  game.perform_action(0, Action(RED));
  game.get_view(view, 0);
  REQUIRE(view.legal_actions().size() == 4);
  game.get_view(view, 1);
  REQUIRE(view.legal_actions().size() == 3);
  game.perform_action(1, Action(GREEN));
  game.get_view(view, 1);
  REQUIRE(view.legal_actions().size() == 12);
};
