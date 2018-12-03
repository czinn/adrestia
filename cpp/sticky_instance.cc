#include "sticky_instance.h"

#include "game_rules.h"
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
	: amount(sticky.get_stacks() ? 1 : invoker.get_amount())
	, remaining_duration(invoker.get_duration())
	, spell(spell)
	, sticky(sticky) {}

StickyInstance::StickyInstance(const GameRules &rules, const json &j)
	: amount(j.at("amount"))
	, remaining_duration(j.at("remaining_duration"))
	, spell(rules.get_spell(j.at("spell_id")))
	, sticky(rules.get_sticky(j.at("sticky_id"))) {}

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
template<bool emit_events>
std::vector<EffectInstance> _apply_to_effect(
		StickyInstance &s,
		size_t player_id,
		EffectInstance &effect,
		size_t sticky_index,
		std::vector<json> &events_out) {
	int initial_amount = s.amount;
	Duration initial_duration = s.remaining_duration;
	switch (s.sticky.get_kind()) {
		case SK_DELTA:
			effect.amount += s.amount;
			break;
		case SK_SHIELD:
			// TODO: charles: This could be made more general by allowing negative
			// shields, but until we actually have a spell that does that, I'm not
			// going to bother implementing it. The code here assumes that state is
			// positive.
			if (s.amount >= std::abs(effect.amount)) {
				s.amount -= std::abs(effect.amount);
				effect.amount = 0;
			} else if (effect.amount > 0) {
				effect.amount -= s.amount;
				s.amount = 0;
			} else {
				effect.amount += s.amount;
				s.amount = 0;
			}
			break;
		case SK_DAMPER:
			if (s.amount >= std::abs(effect.amount)) {
				effect.amount = 0;
			} else if (effect.amount > 0) {
				effect.amount -= s.amount;
			} else {
				effect.amount += s.amount;
			}
			break;
		case SK_SUPER_SHIELD:
			effect.amount = 0;
			s.remaining_duration.set_to_zero();
			break;
		case SK_ID:
			// Do nothing!
			break;
	}
	if (emit_events && initial_amount != s.amount) {
		events_out.emplace_back(json{
			{"type", "sticky_amount_changed"},
			{"player", player_id},
			{"sticky_index", sticky_index},
			{"amount", s.amount}
		});
	}
	if (emit_events && !(initial_duration == s.remaining_duration)) {
		events_out.emplace_back(json{
			{"type", "sticky_duration_changed"},
			{"player", player_id},
			{"sticky_index", sticky_index},
			{"duration", s.remaining_duration}
		});
	}
	// Generate additional effects
	std::vector<EffectInstance> effects;
	for (const auto &e : s.sticky.get_effects()) {
		for (int i = 0; i < (s.sticky.get_stacks() ? s.amount : 1); i++) {
			effects.push_back(EffectInstance(player_id, s.spell, e));
		}
	}
	return effects;
}

std::vector<EffectInstance> StickyInstance::apply_to_effect(
		size_t player_id,
		EffectInstance &effect) {
	std::vector<json> unused;
	return _apply_to_effect<false>(*this, player_id, effect, 0, unused);
}
std::vector<EffectInstance> StickyInstance::apply_to_effect(
		size_t player_id,
		EffectInstance &effect,
		size_t sticky_index,
		std::vector<json> &events_out) {
	return _apply_to_effect<true>(*this, player_id, effect, sticky_index, events_out);
}

template<bool emit_events>
std::vector<EffectInstance> _apply_to_spell(
		StickyInstance &s,
		size_t player_id,
		const Spell &spell,
		size_t sticky_index,
		std::vector<json> &events_out) {
	switch (s.sticky.get_kind()) {
		default:
			// Do nothing!
			break;
	}
	// Generate additional effects
	std::vector<EffectInstance> effects;
	for (const auto &e : s.sticky.get_effects()) {
		for (int i = 0; i < (s.sticky.get_stacks() ? s.amount : 1); i++) {
			effects.push_back(EffectInstance(player_id, s.spell, e));
		}
	}
	return effects;
}

std::vector<EffectInstance> StickyInstance::apply_to_spell(
		size_t player_id,
		const Spell &spell) {
	std::vector<json> unused;
	return _apply_to_spell<false>(*this, player_id, spell, 0, unused);
}
std::vector<EffectInstance> StickyInstance::apply_to_spell(
		size_t player_id,
		const Spell &spell,
		size_t sticky_index,
		std::vector<json> &events_out) {
	return _apply_to_spell<true>(*this, player_id, spell, sticky_index, events_out);
}

template<bool emit_events>
std::vector<EffectInstance> _apply_to_turn(
		StickyInstance &s,
		size_t player_id,
		size_t sticky_index,
		std::vector<json> &events_out) {
	switch (s.sticky.get_kind()) {
		default:
			// Do nothing!
			break;
	}
	// Generate additional effects
	std::vector<EffectInstance> effects;
	for (const auto &e : s.sticky.get_effects()) {
		for (int i = 0; i < (s.sticky.get_stacks() ? s.amount : 1); i++) {
			effects.push_back(EffectInstance(player_id, s.spell, e));
		}
	}
	return effects;
}

std::vector<EffectInstance> StickyInstance::apply_to_turn(size_t player_id) {
	std::vector<json> unused;
	return _apply_to_turn<false>(*this, player_id, 0, unused);
}

std::vector<EffectInstance> StickyInstance::apply_to_turn(
		size_t player_id,
		size_t sticky_index,
		std::vector<json> &events_out) {
	return _apply_to_turn<true>(*this, player_id, sticky_index, events_out);
}

//------------------------------------------------------------------------------
// SERIALIZATION
//------------------------------------------------------------------------------
void to_json(json &j, const StickyInstance &sticky) {
	j["amount"] = sticky.amount;
	j["remaining_duration"] = sticky.remaining_duration;
	j["spell_id"] = sticky.spell.get_id();
	j["sticky_id"] = sticky.sticky.get_id();
}
