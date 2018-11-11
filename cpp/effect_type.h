/* The type of effect for the purposes of effect classification. This is used
 * by Selector to determine whether it applies to an effect or spell. */

#pragma once

#include "json.h"

using json = nlohmann::json;

enum EffectType {
	ET_ATTACK,
	ET_SHIELD,
	ET_SPECIAL,
	ET_POISON,
	ET_CONSTANT,
	ET_COUNTER,
	ET_TECH,
	ET_HEAL,
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
