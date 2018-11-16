#include "sticky_instance.h"

#include "effect_instance.h"
#include "spell.h"
#include "sticky.h"
#include "sticky_invoker.h"

//------------------------------------------------------------------------------
// C++ SEMANTICS
//------------------------------------------------------------------------------
StickyInstance::StickyInstance(
		const Spell &spell,
		const Sticky &sticky,
		const StickyInvoker &invoker)
	: amount(invoker.get_amount())
	, remaining_duration(invoker.get_duration())
	, spell(spell)
	, sticky(sticky) {}

bool StickyInstance::operator==(const StickyInstance &other) const {
	return
		(  this->amount == other.amount
		&& this->remaining_duration == other.remaining_duration
		&& this->spell == other.spell
		&& this->sticky == other.sticky
		);
}

//------------------------------------------------------------------------------
// BUSINESS LOGIC
//------------------------------------------------------------------------------
std::vector<EffectInstance> StickyInstance::apply(size_t player_id, EffectInstance &effect) {
	switch (sticky.get_kind()) {
		case SK_DELTA:
			effect.amount += amount;
			break;
		case SK_SHIELD:
			// TODO: charles: This could be made more general by allowing negative
			// shields, but until we actually have a spell that does that, I'm not
			// going to bother implementing it. The code here assumes that state is
			// positive.
			if (amount >= std::abs(effect.amount)) {
				amount -= std::abs(effect.amount);
				effect.amount = 0;
			} else if (effect.amount > 0) {
				effect.amount -= amount;
				amount = 0;
			} else {
				effect.amount += amount;
				amount = 0;
			}
			break;
		case SK_DAMPER:
			if (amount >= std::abs(effect.amount)) {
				effect.amount = 0;
			} else if (effect.amount > 0) {
				effect.amount -= amount;
			} else {
				effect.amount += amount;
			}
			break;
		case SK_SUPER_SHIELD:
			effect.amount = 0;
			remaining_duration.set_to_zero();
			break;
		case SK_ID:
			// Do nothing!
			break;
	}
	// Generate additional effects
	std::vector<EffectInstance> effects;
	for (const auto &e : sticky.get_effects()) {
		effects.push_back(EffectInstance(player_id, spell, e));
	}
	return effects;
}

std::vector<EffectInstance> StickyInstance::apply(size_t player_id, const Spell &spell) {
	switch (sticky.get_kind()) {
		default:
			// Do nothing!
			break;
	}
	// Generate additional effects
	std::vector<EffectInstance> effects;
	for (const auto &e : sticky.get_effects()) {
		effects.push_back(EffectInstance(player_id, this->spell, e));
	}
	return effects;
}

std::vector<EffectInstance> StickyInstance::apply(size_t player_id) {
	switch (sticky.get_kind()) {
		default:
			// Do nothing!
			break;
	}
	// Generate additional effects
	std::vector<EffectInstance> effects;
	for (const auto &e : sticky.get_effects()) {
		effects.push_back(EffectInstance(player_id, spell, e));
	}
	return effects;
}

//------------------------------------------------------------------------------
// SERIALIZATION
//------------------------------------------------------------------------------
void to_json(json &j, const StickyInstance &sticky) {
	j["amount"] = sticky.amount;
	j["remaining_duration"] = sticky.remaining_duration;
	j["spell"] = sticky.spell.get_id();
	j["sticky"] = sticky.sticky;
}
