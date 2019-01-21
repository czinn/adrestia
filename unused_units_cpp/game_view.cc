#include "game_view.h"

#include "action.h"
#include "colour.h"
#include "json.h"


GameView::GameView() {}


void to_json(json &j, const GameView &view) {
	j["view_index"] = view.view_index;
	j["view_player"] = view.view_player;
	j["players"] = view.players;
	j["turn"] = view.turn;
	j["action_log"] = view.action_log;
}

void GameView::generate_build_units_actions(
	std::vector<Action> &actions,  // Possible actions (accumulator)
	std::vector<std::string> &units,
	int coins,
	int max_units,
	std::map<std::string, UnitKind>::const_iterator begin,
	std::map<std::string, UnitKind>::const_iterator end
) const {
	if (begin == end || coins == 0 || max_units == 0) {
		actions.push_back(Action(units));
		return;
	}

	// XTODO: dmitri: I don't understand this function
	// jim: After reading, I believe this code produces all possible BUILD_UNITS
	// actions that build at most [max_units] units, cost at most [coins] coins,
	// and use unit kinds that come from [begin, end]. It does it in a clever way
	// that avoids duplicates by making all generated sequences of units of
	// non-descending UnitKind (order determined by begin and end).

	auto next_it = begin;
	next_it++;
	if (begin->second.get_tech() != nullptr
			&& view_player.tech.includes(*begin->second.get_tech())
			&& begin->second.get_cost() <= coins) {
		units.push_back(begin->first);
		generate_build_units_actions(
			actions, units, coins - begin->second.get_cost(),
			max_units - 1, begin, end
		);
		units.pop_back();
	}
	generate_build_units_actions(actions, units, coins, max_units, next_it, end);
}


std::vector<Action> GameView::legal_actions() const {
	/* Generate all legal actions for this View's Player.
	 * The result will be empty if the game is over or we are waiting for other
	 * players to make their moves.
	 * Note that ALL players will have no moves iff the game is over.
	 */

	int total_tech = view_player.tech.red + view_player.tech.green + view_player.tech.blue;
	if (total_tech < turn) {
		// If the player has not selected a tech, tech selection is the only legal move.
		return std::vector<Action>({ RED, GREEN, BLUE });
	}
	else if (action_log[turn - 1][view_index].size() < 2) {
		// The player must select actions.
		std::vector<Action> actions;
		std::vector<std::string> units;
		generate_build_units_actions(
			actions, units, view_player.coins,
			rules->get_unit_cap() - view_player.units.size(),
			rules->get_unit_kinds().begin(), rules->get_unit_kinds().end()
		);
		return actions;
	}
	else {
		// The player has already acted this turn; there is nothing more to be done.
		return std::vector<Action>();
	}
}
