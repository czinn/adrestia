#include "random_strategy.h"
#include <chrono>
#include <random>

RandomStrategy::RandomStrategy() : g(std::chrono::high_resolution_clock::now().time_since_epoch().count()) {}

RandomStrategy::~RandomStrategy() {}

Action RandomStrategy::get_action(const GameView &view) {
  std::vector<Action> actions = view.legal_actions();
  return actions[g() % actions.size()];
}
