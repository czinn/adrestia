/* Description of a spell effect. Note that this is NOT the same as an
 * instantiation of an effect used during the execution of a turn; see
 * EffectInstance for more details. */

#pragma once

#include "sticky.h"
#include "effect_type.h"
#include "json.h"

using json = nlohmann::json;

enum EffectKind {
	// Applies delta to target's health.
	EK_HEALTH,
	// Applies delta to target's tech.
	EK_TECH,
	// Applies delta to target's mana.
	EK_MANA,
	// Applies delta to target's mana regeneration.
	EK_REGEN,
	// Adds a sticky to the target.
	EK_STICKY,
};

NLOHMANN_JSON_SERIALIZE_ENUM(EffectKind, {
	{ EK_HEALTH, "health" },
	{ EK_TECH, "tech" },
	{ EK_MANA, "mana" },
	{ EK_REGEN, "mana_regen" },
	{ EK_STICKY, "sticky" },
});

class Effect {
	public:
		Effect();

		EffectKind get_kind() const;
		bool get_targets_self() const;
		EffectType get_effect_type() const;
		int get_amount() const;
		const Sticky &get_sticky() const;

		friend void from_json(const json &j, Effect &effect);
		friend void to_json(json &j, const Effect &effect);

	private:
		// Broadly, what sort of thing the effect does.
		EffectKind kind;
		// Whether the effect targets the creator (true) or the other player (false).
		bool targets_self;
		// The type of effect for the purposes of effect classification.
		EffectType effect_type;
		// For EffectKinds other thank EK_STICKY, the delta to apply to the
		// corresponding attribute of the target Player.
		int amount;
		// The Sticky that this Effect creates, if any.
		Sticky sticky;
};
