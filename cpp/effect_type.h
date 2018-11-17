/* The type of effect for the purposes of effect classification. This is used
 * by Selector to determine whether it applies to an effect or spell. */

#pragma once

#include "json.h"

using json = nlohmann::json;

enum EffectType {
	ET_ATTACK = 0,
	ET_SHIELD = 1,
	ET_SPECIAL = 2,
	ET_POISON = 3,
	ET_CONSTANT = 4,
	ET_COUNTER = 5,
	ET_TECH = 6,
	ET_HEAL = 7,
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
