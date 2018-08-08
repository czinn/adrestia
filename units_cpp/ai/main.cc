#include <iostream>
#include "../json.h"
#include "../game_state.h"
#include "../game_rules.h"
#include "../action.h"
#include "../game_view.h"
#include "strategy.h"
#include "random_strategy.h"

using json = nlohmann::json;

const int PLAYERS = 2;

int main() {
  GameRules rules("rules.json");
  GameState game(rules, PLAYERS);
  std::vector<Strategy*> strategies;
  for (int i = 0; i < PLAYERS; i++) {
    strategies.push_back(new RandomStrategy());
  }
  GameView view;

  while (game.get_winners().size() == 0) {
    for (int i = 0; i < strategies.size(); i++) {
      // Hardcoded to two actions per player per turn; if game changes, will need to change this.
      for (int j = 0; j < 2; j++) {
        game.get_view(view, i);
        Action a = strategies[i]->get_action(view);
        game.perform_action(i, a);
      }
    }
  }

  std::cout << json(game) << std::endl;

  for (int i = 0; i < strategies.size(); i++) {
    delete strategies[i];
  }

  return 0;
}