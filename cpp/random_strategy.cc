#include "random_strategy.h"
#include <chrono>
#include <random>
#include <iostream>

RandomStrategy::RandomStrategy() : g(std::chrono::high_resolution_clock::now().time_since_epoch().count()) {}

RandomStrategy::~RandomStrategy() {}

GameAction RandomStrategy::get_action(const GameView &view) {
	return view.random_action(g);
}
