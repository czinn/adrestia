/* Instance of a spell effect. This object is piped through StickyInstances in
 * order to obtain the effect that is applied to a Player. */

#pragma once

#include "effect_kind.h"
#include "effect_type.h"
#include "sticky.h"
#include "json.h"

using json = nlohmann::json;

class Player;
class Effect;
class Spell;

class EffectInstance {
	public:
		EffectInstance(size_t caster, const Spell &spell, const Effect &effect);
		EffectInstance(const EffectInstance &);
		bool operator==(const EffectInstance &) const;

		// Applies the effect to the given player.
		void apply(Player &player) const;

		// A from_json method is not needed because this type is never serialized;
		// to_json is provided for debugging purposes.
		friend void to_json(json &j, const Effect &effect);

		EffectKind kind;
		bool targets_self;
		EffectType effect_type;
		int amount;
		Sticky sticky;
		const Spell &spell;
		size_t target_player;
};
