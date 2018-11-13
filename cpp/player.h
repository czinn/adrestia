/* State of a player in an instance of Adrestia. */

#pragma once

#include <vector>

#include "book.h"
#include "spell.h"
#include "sticky.h"
#include "game_rules.h"

class Player {
	public:
		Player(const GameRules &, const std::vector<std::string> books);
		bool operator==(const Player &) const;

		// Returns (Spell, book_idx).
		// first is [nullptr] if spell is not in any of the player's books.
		std::pair<const Spell*, size_t> find_spell(const std::string &spell_id) const;
		int level() const;

		friend void to_json(json &, const Player &);

		int hp;
		int max_hp;
		int mp;
		int mp_regen;
		std::vector<int> tech;
		std::vector<const Book*> books;
		// TODO: Stickies.

	private:
		const GameRules &rules;
};
