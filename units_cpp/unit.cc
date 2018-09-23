#include "json.h"
#include "unit.h"


Unit::Unit(const UnitKind &kind)
	: kind(kind)
	, health(kind.get_health())
	, shields(kind.get_shields())
	, build_time(kind.get_build_time()) {}


Unit::Unit(const Unit &unit)
	: kind(unit.kind)
	, health(unit.health)
	, shields(unit.shields)
	, build_time(unit.build_time) {}


Unit::Unit(const UnitKind &kind, const json &j)
	: kind(kind)
	, health(j["health"])
	, shields(j["shields"])
	, build_time(j["build_time"]) {}


void to_json(json &j, const Unit &unit) {
	j["kind"] = unit.kind.get_id();
	j["health"] = unit.health;
	j["shields"] = unit.shields;
	j["build_time"] = unit.build_time;
}
