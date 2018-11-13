#include "effect_instance.h"

#include "sticky_instance.h"
#include "spell.h"
#include "effect.h"
#include "player.h"

//------------------------------------------------------------------------------
// C++ SEMANTICS
//------------------------------------------------------------------------------
EffectInstance::EffectInstance(size_t caster, const Spell &spell, const Effect &effect)
	: kind(effect.get_kind())
	, targets_self(effect.get_targets_self())
	, effect_type(effect.get_effect_type())
	, amount(effect.get_amount())
	, sticky(effect.get_sticky())
	, spell(spell)
	, target_player(effect.get_targets_self() ? caster : 1 - caster) {}

EffectInstance::EffectInstance(const EffectInstance &effect)
	: kind(effect.kind)
	, targets_self(effect.targets_self)
	, effect_type(effect.effect_type)
	, amount(effect.amount)
	, sticky(effect.sticky)
	, spell(effect.spell)
	, target_player(effect.target_player) {}

bool EffectInstance::operator==(const EffectInstance &other) const {
	return (
		this->kind == other.kind &&
		this->targets_self == other.targets_self &&
		this->effect_type == other.effect_type &&
		this->amount == other.amount &&
		this->sticky == other.sticky &&
		this->spell == other.spell &&
		this->target_player == other.target_player
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
			player.stickies.push_back(StickyInstance(spell, sticky));
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
