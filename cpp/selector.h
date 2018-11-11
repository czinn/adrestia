/* Boolean predicate on a spell/effect. Used to determine whether a Sticky
 * should trigger. */

#pragma once

#include <vector>

#include "effect.h"
#include "json.h"

using json = nlohmann::json;

class Spell;

class Selector {
	public:
		Selector();

		bool is_valid() const;

		bool selects_spell(const Spell &spell) const;
		// bool selects_effect(const EffectInstance &effect) const;

		friend void from_json(const json &j, Selector &selector);
		friend void to_json(json &j, const Selector &selector);
	
	private:
		bool valid;
		std::vector<std::string> book_ids;
		std::vector<std::string> spell_ids;
		std::vector<EffectType> effect_types;
};
