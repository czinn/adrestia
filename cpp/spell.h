/* Description of a spell and all its effects. There is one instance of Spell
 * per type of spell. New instances are not created when players cast spells;
 * instead, spells are referenced by id. */

#pragma once

#include <vector>

#include "effect.h"
#include "json.h"

using json = nlohmann::json;

class Spell {
	public:
		Spell();
		std::string get_id() const;
		std::string get_name() const;
		std::string get_book() const;
		int get_tech() const;
		int get_level() const;
		int get_cost() const;
		std::string get_text() const;
		const std::vector<Effect> get_effects() const;

		friend void from_json(const json &j, Spell &spell);
		friend void to_json(json &j, const Spell &spell);

	private:
		std::string id;
		std::string name;
		std::string book;
		int tech; // Required amount of tech in the spell's book.
		int level; // Required total amount of tech.
		int cost; // Mana cost.
		std::string text; // Human-readable description of the spell's effects.
		std::vector<Effect> effects;
};
