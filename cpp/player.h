/* State of a player in an instance of Adrestia. */

#pragma once

#include <vector>
#include <list>

#include "book.h"
#include "spell.h"
#include "sticky.h"
#include "game_rules.h"
#include "sticky_instance.h"

class Player {
	public:
		Player(const GameRules &, const std::vector<std::string> books);
		bool operator==(const Player &) const;

		// Returns (Spell, book_idx).
		// first is [nullptr] if spell is not in any of the player's books.
		std::pair<const Spell*, size_t> find_spell(const std::string &spell_id) const;
		size_t find_book_idx(const std::string &book_id) const;
		int level() const;
		// Applies all triggered stickies to the effect.
		std::vector<EffectInstance> pipe_effect(size_t player_id, EffectInstance &effect, bool inbound);
    // Gets effects from stickies triggered by the spell.
		std::vector<EffectInstance> pipe_spell(size_t player_id, const Spell &spell);
    // Gets effects from stickies that trigger at end of turn. This isn't
    // really piping anything, but it's called pipe_turn for consistency with
    // pipe_effect and pipe_spell.
		std::vector<EffectInstance> pipe_turn(size_t player_id);

    // These functions subtract a step or turn from all stickies, removing them
    // if they are no longer active.
    void subtract_step();
    void subtract_turn();

		friend void to_json(json &, const Player &);

		int hp;
		int max_hp;
		int mp;
		int mp_regen;
		std::vector<int> tech;
		std::vector<const Book*> books;
		std::list<StickyInstance> stickies;

	private:
		const GameRules &rules;
};
