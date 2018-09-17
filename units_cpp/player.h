/* Contains the class Player, which represents a single Adrestia player (along with all their units and resources). */

#pragma once

#include <map>
#include <memory>
#include <vector>

#include "game_rules.h"
#include "tech.h"
#include "unit.h"
#include "unit_kind.h"


class Player {
	public:
		Player();
		Player(const GameRules &rules);
		Player(const Player &player);
		Player(const GameRules &rules, const json &j);
		Player &operator=(Player &player);

		void build_unit(const UnitKind &kind);
		void begin_turn();  // Ask this player to submit orders for the next turn

		friend void to_json(json &j, const Player &player);

		std::map<int, Unit> units;
		bool alive;
		int coins;
		Tech tech;  // Current techs
		int next_unit;  // The next unit will be the nth unit
};
