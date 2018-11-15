/* A game of Adrestia. */

#pragma once

#include <vector>
#include <deque>

#include "player.h"
#include "spell.h"
#include "game_rules.h"
#include "game_action.h"

class GameState {
	public:
		GameState(
			const GameRules &rules,
			const std::vector<std::vector<std::string>> &player_books);
		bool operator==(const GameState &) const;

		bool is_valid_action(size_t player_id, GameAction action) const;
		bool simulate(std::vector<GameAction> actions);
		int turn_number() const; // First turn is 1.
		std::vector<size_t> winners() const; // empty: Game still in progress.

		const std::vector<Player> &get_players() const;
		std::vector<Player> &get_players_for_test();

		friend void to_json(json &, const GameState &);

		std::vector<std::vector<GameAction>> history;
		std::vector<Player> players;

	private:
		const GameRules &rules;

    void process_effect_queue(
        std::deque<EffectInstance> *effect_queue,
        std::deque<EffectInstance> *next_effect_queue);
};
