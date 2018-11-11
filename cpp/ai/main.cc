#include <iostream>
#include "../json.h"
#include "../game_state.h"
#include "../game_rules.h"
#include "../action.h"
#include "../game_view.h"
#include "strategy.h"
#include "monte_strategy.h"
#include "turret_strategy.h"

using json = nlohmann::json;

int main() {
  GameRules rules("rules.json");
  GameState game(rules, 2);
  std::vector<Strategy*> strategies;
  strategies.push_back(new MonteStrategy());
  strategies.push_back(new MonteStrategy());
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
