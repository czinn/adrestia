#pragma once
#include "../action.h"
#include "../game_view.h"
#include "strategy.h"

class TurretStrategy : public Strategy {
 public:
  TurretStrategy();
  virtual ~TurretStrategy();
  virtual Action get_action(const GameView &view);
};
