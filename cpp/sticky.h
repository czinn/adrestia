/* Description of a sticky (i.e. something that affects effects or other game
 * elements and lasts for some duration). Note that this is NOT the same as an
 * instantiation of a sticky attached to a Player object; see StickyInstance
 * for more details. */

#pragma once

#include <vector>

#include "duration.h"
#include "selector.h"
#include "json.h"

using json = nlohmann::json;

enum StickyKind {
	// Modifies the amount field of the effect.
	SK_DELTA,
	// Reduces the amount field of the effect, up to a maximum that is fixed for
	// the lifetime of the shield.
	SK_SHIELD,
  // Reduces the amount field of the effect by a fixed amount.
  SK_DAMPER,
  // Reduces the amount field of the effect to zero, and destroys the sticky.
  SK_SUPER_SHIELD,
	// Does not affect the effect or spell for which this sticky triggers. The
	// name comes from the identity function.
	SK_ID,
};

NLOHMANN_JSON_SERIALIZE_ENUM(StickyKind, {
	{ SK_DELTA, "delta" },
	{ SK_SHIELD, "shield" },
  { SK_DAMPER, "damper" },
  { SK_SUPER_SHIELD, "super_shield" },
	{ SK_ID, "id" },
});

enum TriggerType {
	// Triggers when a spell matching trigger_selector is cast.
	TRIGGER_SPELL,
	// Triggers when an effect is inbound/outbound (see inbound field).
	TRIGGER_EFFECT,
	// Triggers at the end of the turn. 
	TRIGGER_TURN,
};

class Effect;
class Spell;
class EffectInstance;

class Sticky {
	public:
		Sticky();
		bool operator==(const Sticky &) const;

		StickyKind get_kind() const;
		int get_amount() const;
		const std::vector<Effect> get_effects() const;
		Duration get_duration() const;
		bool get_trigger_inbound() const;

		// EffectInstance is not yet implemented.
		bool triggers_for_effect(const EffectInstance &effect, bool inbound) const;
		bool triggers_for_spell(const Spell &spell) const;
		bool triggers_at_end_of_turn() const;

		friend void from_json(const json &j, Sticky &sticky);
		friend void to_json(json &j, const Sticky &sticky);

	private:
		StickyKind kind;
		int amount;
		std::vector<Effect> effects;
		Duration duration;
		TriggerType trigger_type;
		// If trigger_type is not TRIGGER_TURN, used to determine whether the
		// Sticky triggers in response to a given spell/effect.
		Selector trigger_selector;
		// If trigger_type is TRIGGER_EFFECT, indicates whether the Sticky triggers
		// for inbound effects (versus outbound effects).
		bool trigger_inbound;
};

