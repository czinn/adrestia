/* Instance of a spell effect. This object is piped through StickyInstances in
 * order to obtain the effect that is applied to a Player. */

#pragma once

#include "effect_type.h"
#include "spell.h"
#include "effect.h"
#include "player.h"
#include "json.h"

using json = nlohmann::json;

class EffectInstance {
	public:
		EffectInstance(const Spell &spell, const Effect &effect);
		bool operator==(const EffectInstance &) const;

		// Applies the effect to the given player.
		void apply(Player &player) const;

		// A from_json method is not needed because this type is never serialized;
		// to_json is provided for debugging purposes.
		friend void to_json(json &j, const Effect &effect);

		EffectKind kind;
		EffectType effect_type;
		int amount;
		Sticky sticky;
		const Spell &spell;
};
