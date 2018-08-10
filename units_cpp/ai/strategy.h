#pragma once
#include "../action.h"
#include "../game_view.h"

class Strategy {
 public:
  virtual Action get_action(const GameView &view) = 0;
  virtual ~Strategy() {};
};
