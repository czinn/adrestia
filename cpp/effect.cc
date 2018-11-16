#include "effect.h"

//------------------------------------------------------------------------------
// C++ SEMANTICS
//------------------------------------------------------------------------------
Effect::Effect() {}

bool Effect::operator==(const Effect &other) const {
	return
		(  this->kind == other.kind
		&& this->targets_self == other.targets_self
		&& this->effect_type == other.effect_type
		&& this->amount == other.amount
		&& (this->kind != EK_STICKY || this->sticky_invoker == other.sticky_invoker)
		);
}

//------------------------------------------------------------------------------
// GETTERS
//------------------------------------------------------------------------------
EffectKind Effect::get_kind() const { return kind; }
bool Effect::get_targets_self() const { return targets_self; }
EffectType Effect::get_effect_type() const { return effect_type; }
int Effect::get_amount() const { return amount; }
const StickyInvoker &Effect::get_sticky_invoker() const { return sticky_invoker; }

//------------------------------------------------------------------------------
// SERIALIZATION
//------------------------------------------------------------------------------
void from_json(const json &j, Effect &effect) {
	effect.kind = j.at("kind");
	effect.targets_self = j.at("self");
	effect.effect_type = j.at("effect_type");
	effect.amount = j.find("amount") != j.end() ? j.at("amount").get<int>() : 0;
	if (j.find("sticky") != j.end()) {
		effect.sticky_invoker = j.at("sticky");
	}
}

void to_json(json &j, const Effect &effect) {
	j["kind"] = effect.kind;
	j["self"] = effect.targets_self;
	j["effect_type"] = effect.effect_type;
	if (effect.amount != 0) {
		j["amount"] = effect.amount;
	}
	if (effect.kind == EK_STICKY) {
		j["sticky"] = effect.sticky_invoker;
	}
}
