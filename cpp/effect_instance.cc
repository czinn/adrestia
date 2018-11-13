#include "effect_instance.h"

//------------------------------------------------------------------------------
// C++ SEMANTICS
//------------------------------------------------------------------------------
EffectInstance::EffectInstance(const Spell &spell, const Effect &effect)
	: kind(effect.get_kind())
	, effect_type(effect.get_effect_type())
	, amount(effect.get_amount())
	, sticky(effect.get_sticky())
	, spell(spell) {}

bool EffectInstance::operator==(const EffectInstance &other) const {
	return (
		this->kind == other.kind &&
		this->effect_type == other.effect_type &&
		this->amount == other.amount &&
		this->sticky == other.sticky &&
		this->spell == other.spell
	);
}

//------------------------------------------------------------------------------
// BUSINESS LOGIC
//------------------------------------------------------------------------------
void EffectInstance::apply(Player &player) const {
	switch (kind) {
		case EK_TECH:
			player.tech[player.find_book_idx(spell.get_book())] += amount;
			break;
		case EK_HEALTH:
			player.hp += amount;
			break;
		case EK_MANA:
			player.mp += amount;
			break;
		case EK_REGEN:
			player.mp_regen += amount;
			break;
		case EK_STICKY:
			// TODO: Stickies should exist.
			break;
	}
}

//------------------------------------------------------------------------------
// SERIALIZATION
//------------------------------------------------------------------------------
void to_json(json &j, const EffectInstance &effect) {
	j["kind"] = effect.kind;
	j["effect_type"] = effect.effect_type;
	if (effect.amount != 0) {
		j["amount"] = effect.amount;
	}
	if (effect.kind == EK_STICKY) {
		j["sticky"] = effect.sticky;
	}
	j["spell_id"] = effect.spell.get_id();
}
