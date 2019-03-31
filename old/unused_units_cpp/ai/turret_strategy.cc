#include "turret_strategy.h"
#include "../action.h"
#include "../colour.h"

TurretStrategy::TurretStrategy() {}

TurretStrategy::~TurretStrategy() {}

Action TurretStrategy::get_action(const GameView &view) {
  int total_tech = view.view_player.tech.red + view.view_player.tech.green + view.view_player.tech.blue;
  if (total_tech < view.turn) {
    return Action(GREEN);
  } else {
    std::vector<std::string> units;
    for (int i = 2; i < view.view_player.coins; i += 2) {
      units.push_back("turret");
    }
    return Action(units);
  }
}
