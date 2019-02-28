#include "effect.h"

//------------------------------------------------------------------------------
// C++ SEMANTICS
//------------------------------------------------------------------------------
Effect::Effect() : on_hit(nullptr) {}

Effect::Effect(const Effect &effect)
  : kind(effect.kind)
  , targets_self(effect.targets_self)
  , effect_type(effect.effect_type)
  , amount(effect.amount)
  , sticky_invoker(effect.sticky_invoker)
  , on_hit(effect.on_hit != nullptr ? new Effect(*effect.on_hit) : nullptr) {
}

Effect& Effect::operator=(Effect other) {
  std::swap(kind, other.kind);
  std::swap(targets_self, other.targets_self);
  std::swap(effect_type, other.effect_type);
  std::swap(amount, other.amount);
  std::swap(sticky_invoker, other.sticky_invoker);
  std::swap(on_hit, other.on_hit);

  return *this;
}

Effect::~Effect() {
  if (on_hit != nullptr) {
    delete on_hit;
  }
}

bool Effect::operator==(const Effect &other) const {
	return
		(  this->kind == other.kind
		&& this->targets_self == other.targets_self
		&& this->effect_type == other.effect_type
		&& this->amount == other.amount
		&& (this->kind != EK_STICKY || this->sticky_invoker == other.sticky_invoker)
		&& ((this->on_hit == other.on_hit) || (this->on_hit && other.on_hit && *this->on_hit == *other.on_hit))
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
const Effect *Effect::get_on_hit() const { return on_hit; }

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
  if (j.find("on_hit") != j.end()) {
    effect.on_hit = new Effect();
    *effect.on_hit = j.at("on_hit");
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
  if (effect.on_hit != nullptr) {
    j["on_hit"] = *effect.on_hit;
  }
}
