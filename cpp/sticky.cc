#include "sticky.h"

#include "effect.h"

//------------------------------------------------------------------------------
// CONSTRUCTORS
//------------------------------------------------------------------------------
Sticky::Sticky() {}

//------------------------------------------------------------------------------
// GETTERS
//------------------------------------------------------------------------------
StickyKind Sticky::get_kind() const { return kind; }
int Sticky::get_amount() const { return amount; }
const std::vector<Effect> Sticky::get_effects() const { return effects; }

//------------------------------------------------------------------------------
// SERIALIZATION
//------------------------------------------------------------------------------
void from_json(const json &j, Sticky &sticky) {
	sticky.kind = j["kind"];
	sticky.amount = j.find("amount") != j.end() ? j["amount"].get<int>() : 0;
	sticky.duration = j.at("duration");
	/*for (auto it = j["effects"].begin(), end = j["effects"].end(); it != end; it++) {
		sticky.effects.push_back(*it);
	}*/
}

void to_json(json &j, const Sticky &sticky) {
	j["kind"] = sticky.kind;
	if (sticky.amount != 0) {
		j["amount"] = sticky.amount;
	}
	j["duration"] = sticky.duration;
	/*for (auto it = sticky.effects.begin(); it != sticky.effects.end(); it++) {
		j["effects"].push_back(*it);
	}*/
}
