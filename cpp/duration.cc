#include "duration.h"

//------------------------------------------------------------------------------
// C++ SEMANTICS
//------------------------------------------------------------------------------
Duration::Duration() {}

bool Duration::operator==(const Duration &other) const {
	return
		(  this->unit == other.unit
		&& (this->unit != FULL_GAME || this->value == other.value)
		);
}

//------------------------------------------------------------------------------
// GETTERS
//------------------------------------------------------------------------------
bool Duration::is_active() const { return value > 0; }
DurationUnit Duration::get_unit() const { return unit; }
int Duration::get_value() const { return value; }

//------------------------------------------------------------------------------
// MUTATORS
//------------------------------------------------------------------------------
void Duration::subtract_step() {
	if (unit == STEPS) {
		value--;
	}
}

void Duration::subtract_turn() {
	if (unit == STEPS) {
		value = 0;
	} else if (unit == TURNS) {
		value--;
	}
}

//------------------------------------------------------------------------------
// SERIALIZATION
//------------------------------------------------------------------------------
void from_json(const json &j, Duration &duration) {
	if (j.find("steps") != j.end()) {
		duration.unit = STEPS;
		duration.value = j.at("steps");
	} else if (j.find("turns") != j.end()) {
		duration.unit = TURNS;
		duration.value = j.at("turns");
	} else if (j.find("game") != j.end()) {
		duration.unit = FULL_GAME;
		duration.value = 1;
	} else {
		duration.value = 0;
	}
}

void to_json(json &j, const Duration &duration) {
	if (duration.unit == STEPS) {
		j["steps"] = duration.value;
	} else if (duration.unit == TURNS) {
		j["turns"] = duration.value;
	} else if (duration.unit == FULL_GAME) {
		j["game"] = true;
	}
}
