#include "selector.h"
#include "spell.h"

#include <algorithm>

//------------------------------------------------------------------------------
// CONSTRUCTORS
//------------------------------------------------------------------------------
Selector::Selector() : valid(false) {}

//------------------------------------------------------------------------------
// GETTERS
//------------------------------------------------------------------------------
bool Selector::is_valid() const { return valid; }

//------------------------------------------------------------------------------
// PREDICATES
//------------------------------------------------------------------------------
bool Selector::selects_spell(const Spell &spell) const {
	for (auto it = spell.get_effects().begin(); it != spell.get_effects().end(); it++) {
		if (std::find(effect_types.begin(), effect_types.end(), it->get_effect_type())
						!= effect_types.end()) {
			return true;
		}
	}
	return
		std::find(book_ids.begin(), book_ids.end(), spell.get_book())
				!= book_ids.end()
		|| std::find(spell_ids.begin(), spell_ids.end(), spell.get_id())
				!= spell_ids.end();
}

/* EffectInstance is not yet implemented.
bool selects_effect(const EffectInstance &effect) const {
	return
		std::find(book_ids.begin(), book_ids.end(), effect.get_spell().get_book())
				!= book_ids.end()
		|| std::find(spell_ids.begin(), spell_ids.end(), effect.get_spell().get_id())
				!= spell_ids.end()
		|| std::find(effect_types.begin(), effect_types.end(), effect.get_effect_type())
				!= effect_types.end();
}
*/

//------------------------------------------------------------------------------
// SERIALIZATION
//------------------------------------------------------------------------------
void from_json(const json &j, Selector &selector) {
	for (auto it = j["book_ids"].begin(), end = j["book_ids"].end(); it != end; it++) {
		selector.book_ids.push_back(*it);
	}
	for (auto it = j["spell_ids"].begin(), end = j["spell_ids"].end(); it != end; it++) {
		selector.spell_ids.push_back(*it);
	}
	for (auto it = j["effect_types"].begin(), end = j["effect_types"].end(); it != end; it++) {
		selector.effect_types.push_back(*it);
	}
	selector.valid = true;
}

void to_json(json &j, const Selector &selector) {
	if (selector.valid) {
		j["book_ids"] = selector.book_ids;
		j["spell_ids"] = selector.spell_ids;
		j["effect_types"] = selector.effect_types;
	}
}
