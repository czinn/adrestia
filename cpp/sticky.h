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
	// Does not affect the effect or spell for which this sticky triggers. The
	// name comes from the identity function.
	SK_ID = 0,
	// Modifies the amount field of the effect.
	SK_DELTA = 1,
	// Reduces the amount field of the effect, up to a maximum that is fixed for
	// the lifetime of the shield.
	SK_SHIELD = 2,
	// Reduces the amount field of the effect by a fixed amount.
	SK_DAMPER = 3,
	// Reduces the amount field of the effect to zero, and destroys the sticky.
	SK_SUPER_SHIELD = 4,
};

NLOHMANN_JSON_SERIALIZE_ENUM(StickyKind, {
	{ SK_ID, "id" },
	{ SK_DELTA, "delta" },
	{ SK_SHIELD, "shield" },
	{ SK_DAMPER, "damper" },
	{ SK_SUPER_SHIELD, "super_shield" },
});

enum TriggerType {
	// Triggers when a spell matching trigger_selector is cast.
	TRIGGER_SPELL = 0,
	// Triggers when an effect is inbound/outbound (see inbound field).
	TRIGGER_EFFECT = 1,
	// Triggers at the end of the turn. 
	TRIGGER_TURN = 2,
};

class Effect;
class Spell;
class EffectInstance;

class Sticky {
	public:
		Sticky();
		bool operator==(const Sticky &) const;

		std::string get_id() const;
		std::string get_name() const;
		std::string get_text() const;
		StickyKind get_kind() const;
		const std::vector<Effect> &get_effects() const;
		bool get_trigger_inbound() const;

		// EffectInstance is not yet implemented.
		bool triggers_for_effect(const EffectInstance &effect, bool inbound) const;
		bool triggers_for_spell(const Spell &spell) const;
		bool triggers_at_end_of_turn() const;

		friend void from_json(const json &j, Sticky &sticky);
		friend void to_json(json &j, const Sticky &sticky);

	private:
		std::string id;
		std::string name;
		std::string text;
		StickyKind kind;
		std::vector<Effect> effects;
		TriggerType trigger_type;
		// If trigger_type is not TRIGGER_TURN, used to determine whether the
		// Sticky triggers in response to a given spell/effect.
		Selector trigger_selector;
		// If trigger_type is TRIGGER_EFFECT, indicates whether the Sticky triggers
		// for inbound effects (versus outbound effects).
		bool trigger_inbound;
};

