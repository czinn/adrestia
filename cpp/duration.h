/* Represents a duration for which a Sticky remains active. Also used in
 * StickyInstances to keep track of how much time is left before it expires. */

#pragma once

#include "json.h"

using json = nlohmann::json;

enum DurationUnit {
	FULL_GAME = 0,
	TURNS = 1,
	STEPS = 2,
};

class Duration {
	public:
		Duration();
		bool operator==(const Duration &) const;

		bool is_active() const;
		DurationUnit get_unit() const;
		int get_value() const;

		void subtract_step();
		void subtract_turn();
		void set_to_zero();

		friend void from_json(const json &j, Duration &duration);
		friend void to_json(json &j, const Duration &duration);

	private:
		DurationUnit unit;
		int value;
};
