#pragma once
#include <map>
#include "player.h"

class PlayerView {
 public:
  PlayerView();
  PlayerView(const Player &player);
  PlayerView(const GameRules &rules, const json &j);

  friend void to_json(json &j, const PlayerView &view);

  std::map<int, Unit> units;
  bool alive;
  int coins;
};
