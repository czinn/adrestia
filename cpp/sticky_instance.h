/* Instance of a sticky attached to a player. Its duration ticks down and its
 * amount or other properties may change. */

#pragma once

#include "duration.h"
#include "json.h"

using json = nlohmann::json;

class Spell;
class Sticky;
class EffectInstance;

class StickyInstance {
	public:
		StickyInstance(const Spell &spell, const Sticky &sticky);
		bool operator==(const StickyInstance &) const;

		// Applies the sticky to the effect and possibly generates additional
		// effects. Only effects that trigger the sticky should be passed to this
		// function.
		// player_id is the id of the player to whom the sticky is attached.
		std::vector<EffectInstance> apply(size_t player_id, EffectInstance &effect);

		// state is used differently depending on the kind of sticky. For shields,
		// it is the remaining shield strength.
		int state;
		Duration remaining_duration;
		const Spell &spell;
		const Sticky &sticky;
};
