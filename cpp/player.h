/* State of a player in an instance of Adrestia. */

#pragma once

#include <vector>

#include "book.h"
#include "sticky.h"
#include "game_rules.h"

class Player {
	public:
		Player(const GameRules &, const std::vector<std::string> books);
		bool operator==(const Player &) const;

		friend void to_json(json &, const Player &);

	private:
		int hp;
		int max_hp;
		int mp;
		int mp_regen;
		std::vector<int> tech;
		std::vector<const Book*> books;
		std::vector<const Sticky*> outbound_stickies;
		std::vector<const Sticky*> inbound_stickies;
};
