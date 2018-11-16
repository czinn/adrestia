#include "sticky.h"

#include "effect.h"
#include "spell.h"
#include "effect_instance.h"

//------------------------------------------------------------------------------
// C++ SEMANTICS
//------------------------------------------------------------------------------
Sticky::Sticky() {}

bool Sticky::operator==(const Sticky &other) const {
	return
		(  this->id == other.id
		&& this->name == other.name
		&& this->text == other.text
		&& this->kind == other.kind
		&& this->effects == other.effects
		&& this->trigger_type == other.trigger_type
		&& this->trigger_selector == other.trigger_selector
		&& this->trigger_inbound == other.trigger_inbound
		);
}

//------------------------------------------------------------------------------
// GETTERS
//------------------------------------------------------------------------------
std::string Sticky::get_id() const { return id; }
std::string Sticky::get_name() const { return name; }
std::string Sticky::get_text() const { return text; }
StickyKind Sticky::get_kind() const { return kind; }
const std::vector<Effect> Sticky::get_effects() const { return effects; }

//------------------------------------------------------------------------------
// PREDICATES
//------------------------------------------------------------------------------
bool Sticky::triggers_for_effect(const EffectInstance &effect, bool inbound) const {
	return trigger_type == TRIGGER_EFFECT && trigger_inbound == inbound &&
		trigger_selector.selects_effect(effect);
}

bool Sticky::triggers_for_spell(const Spell &spell) const {
	return trigger_type == TRIGGER_SPELL && trigger_selector.selects_spell(spell);
}

bool Sticky::triggers_at_end_of_turn() const {
	return trigger_type == TRIGGER_TURN;
}

//------------------------------------------------------------------------------
// SERIALIZATION
//------------------------------------------------------------------------------
void from_json(const json &j, Sticky &sticky) {
	sticky.id = j.at("id");
	sticky.name = j.at("name");
	sticky.text = j.at("text");
	sticky.kind = j.at("kind");
	if (j.find("effects") != j.end()) {
		for (auto it = j.at("effects").begin(), end = j.at("effects").end(); it != end; it++) {
			sticky.effects.push_back(*it);
		}
	}
	json t = j.at("trigger");
	sticky.trigger_inbound = false;
	if (t.find("spell") != t.end()) {
		sticky.trigger_type = TRIGGER_SPELL;
		sticky.trigger_selector = t.at("spell");
	} else if (t.find("effect") != t.end()) {
		sticky.trigger_type = TRIGGER_EFFECT;
		sticky.trigger_selector = t.at("effect");
		sticky.trigger_inbound = t.at("inbound");
	} else if (t.find("turn") != t.end()) {
		sticky.trigger_type = TRIGGER_TURN;
	}
}

void to_json(json &j, const Sticky &sticky) {
	j["id"] = sticky.id;
	j["name"] = sticky.name;
	j["text"] = sticky.text;
	j["kind"] = sticky.kind;
	if (sticky.effects.size() != 0) {
		j["effects"] = sticky.effects;
	}
	json t;
	if (sticky.trigger_type == TRIGGER_SPELL) {
		t["spell"] = sticky.trigger_selector;
	} else if (sticky.trigger_type == TRIGGER_EFFECT) {
		t["effect"] = sticky.trigger_selector;
		t["inbound"] = sticky.trigger_inbound;
	} else if (sticky.trigger_type == TRIGGER_TURN) {
		t["turn"] = true;
	}
	j["trigger"] = t;
}
