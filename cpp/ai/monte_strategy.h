#pragma once
#include <random>
#include "../game_action.h"
#include "../game_view.h"
#include "strategy.h"

class MonteStrategy : public Strategy {
 public:
	MonteStrategy();
	virtual ~MonteStrategy();
	virtual GameAction get_action(const GameView &view);
 private:
	std::mt19937 gen;
};
