/* A view of a game of Adrestia. */

#pragma once

#include <vector>
#include <deque>

#include "game_state.h"
#include "player.h"
#include "game_rules.h"
#include "game_action.h"

// Spell id for tech spells whose exact identity are not visible to the
// viewing player.
static std::string tech_spell_id = "tech";

class GameView {
	public:
		GameView(const GameState &state, size_t view_player_id);
		GameView(const GameRules &rules, const json &j);
		bool operator==(const GameView &) const;


		int turn_number() const; // First turn is 1.
		std::vector<size_t> winners() const; // empty: Game still in progress.
		std::vector<GameAction> legal_actions() const;
		std::vector<GameAction> sane_actions() const;

		friend void to_json(json &, const GameView &);

		std::vector<std::vector<GameAction>> history;
		std::vector<Player> players;
		size_t view_player_id;
		const GameRules &rules;

	private:

		void generate_actions(std::vector<GameAction> &, GameAction &, int, int) const;
		void generate_sane_actions(std::vector<GameAction> &, GameAction &, int, int, bool, bool) const;
};
