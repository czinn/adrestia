/* Represents a duration for which a Sticky remains active. Also used in
 * StickyInstances to keep track of how much time is left before it expires. */

#pragma once

#include "json.h"

using json = nlohmann::json;

enum DurationUnit {
	FULL_GAME,
	TURNS,
	STEPS,
};

class Duration {
	public:
		Duration();
		bool is_active() const;
		DurationUnit get_unit() const;
		int get_value() const;

		void subtract_step();
		void subtract_turn();

		friend void from_json(const json &j, Duration &duration);
		friend void to_json(json &j, const Duration &duration);

	private:
		DurationUnit unit;
		int value;
};
