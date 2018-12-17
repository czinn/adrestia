#pragma once
#include <random>
#include "game_action.h"
#include "game_view.h"
#include "strategy.h"
#include <fdeep/fdeep.hpp>

std::vector<double> cfr_state_vector(const GameState &g);

class CfrStrategy : public Strategy {
 public:
	CfrStrategy();
	CfrStrategy(std::vector<double> weights);
	CfrStrategy(fdeep::model *model);
	virtual ~CfrStrategy();
	virtual GameAction get_action(const GameView &view);
 private:
	double score_game_state(const GameState &) const;
	double score_action_pair(
			const GameState &g,
			const GameAction &a0,
			const GameAction &a1,
			std::unordered_map<size_t, double> &score_map) const;
	std::mt19937 gen;
	std::vector<double> weights;
	fdeep::model *model;
};
