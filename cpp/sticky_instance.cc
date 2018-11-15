#include "sticky_instance.h"

#include "effect_instance.h"
#include "spell.h"
#include "sticky.h"

//------------------------------------------------------------------------------
// C++ SEMANTICS
//------------------------------------------------------------------------------
StickyInstance::StickyInstance(const Spell &spell, const Sticky &sticky)
	: remaining_duration(sticky.get_duration())
	, spell(spell)
	, sticky(sticky) {
	switch (sticky.get_kind()) {
		case SK_SHIELD:
			state = sticky.get_amount();
			break;
		default:
			break;
	}
}

bool StickyInstance::operator==(const StickyInstance &other) const {
	return
		(  this->state == other.state
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
			effect.amount += sticky.get_amount();
			break;
		case SK_SHIELD:
			// TODO: charles: This could be made more general by allowing negative
			// shields, but until we actually have a spell that does that, I'm not
			// going to bother implementing it. The code here assumes that state is
			// positive.
			if (state >= std::abs(effect.amount)) {
				state -= std::abs(effect.amount);
				effect.amount = 0;
			} else if (effect.amount > 0) {
				effect.amount -= state;
				state = 0;
			} else {
				effect.amount += state;
				state = 0;
			}
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
    case SK_DELTA:
    case SK_SHIELD:
    case SK_ID:
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
    case SK_DELTA:
    case SK_SHIELD:
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

//------------------------------------------------------------------------------
// SERIALIZATION
//------------------------------------------------------------------------------
void to_json(json &j, const StickyInstance &sticky) {
  j["state"] = sticky.state;
  j["remaining_duration"] = sticky.remaining_duration;
  j["spell"] = sticky.spell.get_id();
  j["sticky"] = sticky.sticky;
}
