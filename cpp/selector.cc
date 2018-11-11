#include "selector.h"
#include "spell.h"

#include <algorithm>

//------------------------------------------------------------------------------
// C++ SEMANTICS
//------------------------------------------------------------------------------
Selector::Selector() : valid(false) {}

bool Selector::operator==(const Selector &other) const {
	return (
		this->valid == other.valid &&
		this->book_ids == other.book_ids &&
		this->spell_ids == other.spell_ids &&
		this->effect_types == other.effect_types
	);
};

//------------------------------------------------------------------------------
// GETTERS
//------------------------------------------------------------------------------
bool Selector::is_valid() const { return valid; }

//------------------------------------------------------------------------------
// PREDICATES
//------------------------------------------------------------------------------
bool Selector::selects_spell(const Spell &spell) const {
	bool effect_type_matches = effect_types.size() == 0;
	for (auto it = spell.get_effects().begin(); it != spell.get_effects().end(); it++) {
		if (std::find(effect_types.begin(), effect_types.end(), it->get_effect_type())
						!= effect_types.end()) {
			effect_type_matches = true;
			break;
		}
	}
	return
		effect_type_matches &&
		(book_ids.size() == 0 ||
		 		std::find(book_ids.begin(), book_ids.end(), spell.get_book()) != book_ids.end()) &&
		(spell_ids.size() == 0 ||
				std::find(spell_ids.begin(), spell_ids.end(), spell.get_id()) != spell_ids.end());
}

/* EffectInstance is not yet implemented.
bool selects_effect(const EffectInstance &effect) const {
	return
		(book_ids.size() == 0 ||
				std::find(book_ids.begin(), book_ids.end(),
						effect.get_spell().get_book()) != book_ids.end()) &&
		(spell_ids.size() == 0 ||
				std::find(spell_ids.begin(), spell_ids.end(),
						effect.get_spell().get_id()) != spell_ids.end()) &&
		(effect_types.size() == 0 ||
				std::find(effect_types.begin(), effect_types.end(),
						effect.get_effect_type()) != effect_types.end());
}
*/

//------------------------------------------------------------------------------
// SERIALIZATION
//------------------------------------------------------------------------------
void from_json(const json &j, Selector &selector) {
	if (j.find("book_id") != j.end()) {
		for (auto it = j.at("book_id").begin(), end = j.at("book_id").end(); it != end; it++) {
			selector.book_ids.push_back(*it);
		}
	}
	if (j.find("spell_id") != j.end()) {
		for (auto it = j.at("spell_id").begin(), end = j.at("spell_id").end(); it != end; it++) {
			selector.spell_ids.push_back(*it);
		}
	}
	if (j.find("effect_type") != j.end()) {
		for (auto it = j.at("effect_type").begin(), end = j.at("effect_type").end(); it != end; it++) {
			selector.effect_types.push_back(*it);
		}
	}
	selector.valid = true;
}

void to_json(json &j, const Selector &selector) {
	if (selector.valid) {
		j = json::object();
		if (selector.book_ids.size() > 0) {
			j["book_id"] = selector.book_ids;
		}
		if (selector.spell_ids.size() > 0) {
			j["spell_id"] = selector.spell_ids;
		}
		if (selector.effect_types.size() > 0) {
			j["effect_type"] = selector.effect_types;
		}
	}
}
