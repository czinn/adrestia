/* Instance of a spell effect. This object is piped through StickyInstances in
 * order to obtain the effect that is applied to a Player. */

#pragma once

#include "effect_kind.h"
#include "effect_type.h"
#include "sticky_invoker.h"
#include "json.h"
#include "game_rules.h"

using json = nlohmann::json;

class Player;
class Effect;
class Spell;

class EffectInstance {
	public:
		EffectInstance(size_t caster, const Spell &spell, const Effect &effect);
		EffectInstance(const EffectInstance &);
		bool operator==(const EffectInstance &) const;

		// Whether the effect is now a no-op. This can happen if it gets completely
		// blocked by a shield, for example. The effect should not continue
		// propagating if this happens.
		bool fizzles() const;
		// Applies the effect to the given player.
		void apply(const GameRules &rules, Player &player) const;

		// A from_json method is not needed because this type is never serialized;
		// to_json is provided for debugging purposes.
		friend void to_json(json &j, const EffectInstance &effect);

		EffectKind kind;
		bool targets_self;
		EffectType effect_type;
		int amount;
		const StickyInvoker &sticky_invoker;
		const Spell &spell;
    const Effect &effect;
		size_t target_player;
};
