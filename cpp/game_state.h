/* A game of Adrestia. */

#pragma once

#include <vector>
#include <deque>

#include "player.h"
#include "game_rules.h"
#include "game_action.h"

class GameView;

class GameState {
	public:
		GameState(
			const GameRules &rules,
			const std::vector<std::vector<std::string>> &player_books);
		GameState(const GameRules &rules, const json &j);
		// Creates a determinization of the game view, with the tech and books for
		// the other player given by the other parameters.
		GameState(const GameView &view, std::vector<int> &tech, std::vector<const Book*> &books);
		bool operator==(const GameState &) const;
		// Pretty print for debugging purposes.
		friend std::ostream &operator<<(std::ostream &os, const GameState &);

		template<bool emit_events>
		friend bool simulate_base(GameState &, std::vector<GameAction> actions, std::vector<json> &events_out);

		bool is_valid_action(size_t player_id, GameAction action) const;
		bool simulate(const std::vector<GameAction> &actions);
		bool simulate(const std::vector<GameAction> &actions, std::vector<json> &events_out);
		int turn_number() const; // First turn is 1.
		std::vector<size_t> winners() const; // empty: Game still in progress.

		friend void to_json(json &, const GameState &);

		std::vector<std::vector<GameAction>> history;
		std::vector<Player> players;
		const GameRules &rules;

	private:
		void process_effect_queue(
				std::deque<EffectInstance> *effect_queue,
				std::deque<EffectInstance> *next_effect_queue);
};
