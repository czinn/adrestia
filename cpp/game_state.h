/* A game of Adrestia. */

#pragma once

#include <vector>

#include "player.h"
#include "game_rules.h"

class GameState {
	public:
		GameState(
			const GameRules &rules,
			const std::vector<std::vector<std::string>> &player_books);
		bool operator==(const GameState &) const;

		friend void to_json(json &, const GameState &);

	private:
		const GameRules &rules;
		std::vector<Player> players;
};
