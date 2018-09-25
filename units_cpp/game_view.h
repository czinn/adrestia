/* Contains the class GameView, which contains all data about an Adrestia game
 * as visible to a particular player. */

#pragma once

#include <memory>
#include <vector>

#include "action.h"
#include "battle.h"
#include "game_rules.h"
#include "json.h"
#include "player_view.h"

using nlohmann::json;


class GameView {
	/* Contains all data about an Adrestia game, as seen by a particular player.
	 * It is essentially a GameState with hidden information.
	 */

	public:
		GameView();

		// What actions are CURRENTLY legal for this player? Empty if waiting for
		// other players, or the game is over.
		std::vector<Action> legal_actions() const;

		const GameRules *rules;  // The rules of the game this view reprsents
		int view_index;  // The player number of the player this view represents
		Player view_player;  // The actual player
		std::vector<PlayerView> players;
		std::vector<std::vector<std::vector<Action>>> action_log;
		const std::vector<std::shared_ptr<Battle>> *battles;
		int turn;

		friend void to_json(json &j, const GameView &view);

	private:
		// Recursively generate all BUILD_UNIT actions that this player can perform
		// this turn.
		void generate_build_units_actions(
				std::vector<Action> &actions,  // The list of actions (an accumulator)
				std::vector<std::string> &units,
				int coins,  // List of coins remaining to allocate.
				int max_units,
				std::map<std::string, UnitKind>::const_iterator begin,
				std::map<std::string, UnitKind>::const_iterator end
		) const;
};
