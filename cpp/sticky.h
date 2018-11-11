/* Description of a sticky (i.e. something that affects effects or other game
 * elements and lasts for some duration). Note that this is NOT the same as an
 * instantiation of a sticky attached to a Player object; see StickyInstance
 * for more details. */

#pragma once

#include <vector>

#include "duration.h"
#include "json.h"

using json = nlohmann::json;

enum StickyKind {
	// Modifies the amount field of the effect.
	SK_DELTA,
	// Reduces the amount field of the effect, up to a maximum that is fixed for
	// the lifetime of the shield.
	SK_SHIELD,
	// Does not affect the effect or spell for which this sticky triggers. The
	// name comes from the identity function.
	SK_ID,
};

NLOHMANN_JSON_SERIALIZE_ENUM(StickyKind, {
	{ SK_DELTA, "delta" },
	{ SK_SHIELD, "shield" },
	{ SK_ID, "id" },
});

class Effect;

class Sticky {
	public:
		Sticky();
		StickyKind get_kind() const;
		int get_amount() const;
		const std::vector<Effect> get_effects() const;
		Duration get_duration() const;

		friend void from_json(const json &j, Sticky &sticky);
		friend void to_json(json &j, const Sticky &sticky);

	private:
		StickyKind kind;
		int amount;
		std::vector<Effect> effects;
		Duration duration;
};
