#pragma once
#include <random>
#include "../game_action.h"
#include "../game_view.h"
#include "strategy.h"

class RandomStrategy : public Strategy {
 public:
	RandomStrategy();
	virtual ~RandomStrategy();
	virtual GameAction get_action(const GameView &view);
 private:
	std::mt19937 g;
};
