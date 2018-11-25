#pragma once
#include "game_action.h"
#include "game_view.h"

class Strategy {
 public:
	virtual GameAction get_action(const GameView &view) = 0;
	virtual ~Strategy() {};
};
