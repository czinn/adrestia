/* Defines the class GameState, which contains all information relevant to a
 * single Adrestia game. */


#pragma once

#include <memory>
#include <random>
#include <vector>

#include "action.h"
#include "battle.h"
#include "game_rules.h"
#include "game_view.h"
#include "json.h"
#include "player.h"
#include "player_view.h"
#include "tech.h"

using json = nlohmann::json;


class GameState {
	// Contains all information about a single Adrestia game, including the
	// entire history.
	public:
		GameState(const GameRules &rules, int num_players);
		GameState(const GameRules &rules, const json &j);
		GameState(const GameState &game_state);
		// Determines a state using the given hidden information
		GameState(const GameView &view, const std::vector<Tech> &techs);

		// Only performs legal actions. Returns whether the action was legal.
		bool perform_action(int player, const Action &action);
		void get_view(GameView &view, int player) const;
		// one winner = normal; all players = draw; no winners = game not done.
		std::vector<int> get_winners() const;
		const GameRules &get_rules() const;
		// Turn 0 means uninitialized; turn 1 is the first playable turn.
		int get_turn() const;
		const std::vector<Player> &get_players() const;
		// [turn][player][action]
		const std::vector<std::vector<std::vector<Action>>> &get_action_log() const;
		const std::vector<std::shared_ptr<Battle>> &get_battles() const;

		friend void to_json(json &j, const GameState &game_state);

	private:
		const GameRules &rules;
		std::vector<Player> players;
		// [turn][player][action]
		std::vector<std::vector<std::vector<Action>>> action_log;
		std::vector<std::shared_ptr<Battle>> battles;
		int turn; // 0 means uninitialized; 1 is the first playable turn.
		int players_ready; // Number of players that are ready to begin the battle.

		void begin_turn(); // Start asking players for their orders this turn
		void execute_battle(); // Executes the battle and advances the turn
};
