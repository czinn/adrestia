/* Description of a spell effect. Note that this is NOT the same as an
 * instantiation of an effect used during the execution of a turn; see
 * EffectInstance for more details. */

#pragma once

#include <vector>

#include "json.h"

using json = nlohmann::json;

enum EffectKind {
	EK_HEALTH,
	EK_TECH,
	EK_MANA,
	EK_REGEN,
	EK_STICKY
};

NLOHMANN_JSON_SERIALIZE_ENUM(EffectKind, {
	{ EK_HEALTH, "health" },
	{ EK_TECH, "tech" },
	{ EK_MANA, "mana" },
	{ EK_REGEN, "mana_regen" },
	{ EK_STICKY, "sticky" },
});

enum EffectType {
	ET_ATTACK,
	ET_SHIELD,
	ET_SPECIAL,
	ET_POISON,
	ET_CONSTANT,
	ET_COUNTER,
	ET_TECH,
	ET_HEAL
};

NLOHMANN_JSON_SERIALIZE_ENUM(EffectType, {
	{ ET_ATTACK, "attack" },
	{ ET_SHIELD, "shield" },
	{ ET_SPECIAL, "special" },
	{ ET_POISON, "poison" },
	{ ET_CONSTANT, "constant" },
	{ ET_COUNTER, "counter" },
	{ ET_TECH, "tech" },
	{ ET_HEAL, "heal" },
});

class Effect {
	public:
		Effect();
		EffectKind get_kind() const;
		bool get_targets_self() const;
		EffectType get_effect_type() const;
		int get_amount() const;

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
};
