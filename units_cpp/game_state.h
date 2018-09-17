/* Defines the class GameState, which contains all information relevant to a single Adrestia game. */


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
	/* Contains all information relevant to a single Adrestia game.
	 * Crucially, (am able to reconstruct EVERYTHING about a game) <=> (can construct/have a GameState for the game)
	 */
	public:
		GameState(const GameRules &rules, int num_players);
		GameState(const GameRules &rules, const json &j);  // From GameRules and a json version of a GameState
		GameState(const GameState &game_state);
		GameState(const GameView &view, const std::vector<Tech> &techs);  // Determinization with the given hidden information

		// Returns false if the action is illegal; true otherwise.
		//     In the case of an illegal action, nothing is changed.
		bool perform_action(int player, const Action &action);

		void get_view(GameView &view, int player) const;
		std::vector<int> get_winners() const;  // 1 Winner = normal, all players = draw, no winners = game not done
		const GameRules &get_rules() const;
		int get_turn() const;  // Get current turn. Turn 0 means uninitialized; turn 1 is the first 'playable' turn.
		const std::vector<Player> &get_players() const;
		const std::vector<std::vector<std::vector<Action>>> &get_action_log() const;  // [turn][player][action]
		const std::vector<std::shared_ptr<Battle>> &get_battles() const;

		friend void to_json(json &j, const GameState &game_state);

	private:
		const GameRules &rules;
		std::vector<Player> players;
		std::vector<std::vector<std::vector<Action>>> action_log;  // [turn][player][action]
		std::vector<std::shared_ptr<Battle>> battles;
		int turn;  // Current turn. Turn 0 means uninitialized; turn 1 is the first 'playable' turn.
		int players_ready;  // Number of players that are ready to begin the battle.

		void begin_turn();  // Start asking players for their orders this turn
		void execute_battle();  // Executes the battle and advances the turn
};
