#pragma once
#include <map>
#include <memory>
#include <vector>
#include "game_rules.h"
#include "tech.h"
#include "unit.h"
#include "unit_kind.h"

class Player {
 public:
  Player();
  Player(const GameRules &rules);
  Player(const Player &player);
  Player(const GameRules &rules, const json &j);
  Player &operator=(Player &player);

  void build_unit(const UnitKind &kind);
  void begin_turn();

  friend void to_json(json &j, const Player &player);

  std::map<int, Unit> units;
  bool alive;
  int coins;
  Tech tech;
  int next_unit;
};
