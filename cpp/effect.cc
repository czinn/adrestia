#include "effect.h"

//------------------------------------------------------------------------------
// CONSTRUCTORS
//------------------------------------------------------------------------------
Effect::Effect() {}

//------------------------------------------------------------------------------
// GETTERS
//------------------------------------------------------------------------------
EffectKind Effect::get_kind() const { return kind; }
bool Effect::get_targets_self() const { return targets_self; }
EffectType Effect::get_effect_type() const { return effect_type; }
int Effect::get_amount() const { return amount; }

//------------------------------------------------------------------------------
// SERIALIZATION
//------------------------------------------------------------------------------
void from_json(const json &j, Effect &effect) {
	effect.kind = j["kind"];
	effect.targets_self = j["self"];
	effect.effect_type = j["effect_type"];
	effect.amount = j.find("amount") != j.end() ? j["amount"].get<int>() : 0;
}

void to_json(json &j, const Effect &effect) {
	j["kind"] = effect.kind;
	j["self"] = effect.targets_self;
	j["effect_type"] = effect.effect_type;
	if (effect.amount != 0) {
		j["amount"] = effect.amount;
	}
}
