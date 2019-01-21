/* Contains Unit, the class representing an instance of a particular unit. */

#pragma once

#include "json.h"
#include "unit_kind.h"

using json = nlohmann::json;


class Unit {
	public:
		Unit(const UnitKind &kind);
		Unit(const Unit &unit);
		Unit(const UnitKind &kind, const json &j);

		friend void to_json(json &j, const Unit &unit);

		const UnitKind &kind;
		int health;  // Current health
		int shields;  // Current shields
		int build_time;  // Time remaining
};
