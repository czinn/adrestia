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
	, sticky_invoker(effect.get_sticky_invoker())
	, spell(spell)
	, target_player(effect.get_targets_self() ? caster : 1 - caster) {}

EffectInstance::EffectInstance(const EffectInstance &effect)
	: kind(effect.kind)
	, targets_self(effect.targets_self)
	, effect_type(effect.effect_type)
	, amount(effect.amount)
	, sticky_invoker(effect.sticky_invoker)
	, spell(effect.spell)
	, target_player(effect.target_player) {}

bool EffectInstance::operator==(const EffectInstance &other) const {
	return (
		this->kind == other.kind &&
		this->targets_self == other.targets_self &&
		this->effect_type == other.effect_type &&
		this->amount == other.amount &&
		this->sticky_invoker == other.sticky_invoker &&
		this->spell == other.spell &&
		this->target_player == other.target_player
	);
}

//------------------------------------------------------------------------------
// BUSINESS LOGIC
//------------------------------------------------------------------------------
bool EffectInstance::fizzles() const {
	switch (kind) {
		case EK_TECH:
		case EK_HEALTH:
		case EK_MANA:
		case EK_REGEN:
			return amount == 0;
		case EK_STICKY:
			return false;
	}
}

void EffectInstance::apply(const GameRules &rules, Player &player) const {
	switch (kind) {
		case EK_TECH:
			player.tech[player.find_book_idx(spell.get_book())] += amount;
			break;
		case EK_HEALTH:
			player.hp += amount;
			player.hp = std::min(player.hp, player.max_hp);
			break;
		case EK_MANA:
			player.mp += amount;
			player.mp = std::max(0, player.mp);
			player.mp = std::min(rules.get_mana_cap(), player.mp);
			break;
		case EK_REGEN:
			player.mp_regen += amount;
			break;
		case EK_STICKY:
			player.add_sticky(
					StickyInstance(
						spell,
						rules.get_sticky(sticky_invoker.get_sticky_id()),
						sticky_invoker
					));
			break;
	}
}

//------------------------------------------------------------------------------
// SERIALIZATION
//------------------------------------------------------------------------------
void to_json(json &j, const EffectInstance &effect) {
	j["kind"] = effect.kind;
	j["targets_self"] = effect.targets_self;
	j["effect_type"] = effect.effect_type;
	if (effect.amount != 0) {
		j["amount"] = effect.amount;
	}
	if (effect.kind == EK_STICKY) {
		j["sticky"] = effect.sticky_invoker;
	}
	j["spell_id"] = effect.spell.get_id();
	j["target_player"] = effect.target_player;
}
