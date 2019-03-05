#pragma once
#include <random>
#include <set>
#include "game_action.h"
#include "game_view.h"
#include "strategy.h"

class MonteStrategy : public Strategy {
 public:
  MonteStrategy(int iterations, bool multilevel);
  virtual ~MonteStrategy();
  virtual GameAction get_action(const GameView &view);
 private:
  std::mt19937 gen;
  int iterations = 1;
  bool multilevel = true;
};

std::set<int> choose(std::mt19937 &gen, int n, int k);
