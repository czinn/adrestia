/* Broadly, what sort of thing an effect does. See below for descriptions of
 * the kinds of effects. */

#pragma once

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
