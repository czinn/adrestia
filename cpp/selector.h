/* Boolean predicate on a spell/effect. Used to determine whether a Sticky
 * should trigger. The interpretation of a Selector is that it matches
 * a spell/effect if its book appears in book_ids, AND its spell (source spell
 * for effects) appears in spell_ids, AND its effect_type (any component
 * effect_type) appears in effect_types. For each of the things, an empty list
 * matches anything. */

#pragma once

#include <vector>

#include "effect_type.h"
#include "json.h"

using json = nlohmann::json;

class Spell;
class EffectInstance;

class Selector {
	public:
		Selector();
		bool operator==(const Selector &) const;

		bool is_valid() const;

		bool selects_spell(const Spell &spell) const;
		bool selects_effect(const EffectInstance &effect) const;

		friend void from_json(const json &j, Selector &selector);
		friend void to_json(json &j, const Selector &selector);
	
	private:
		bool valid;
		std::vector<std::string> book_ids;
		std::vector<std::string> spell_ids;
		std::vector<EffectType> effect_types;
};
