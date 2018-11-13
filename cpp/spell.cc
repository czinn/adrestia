#include "spell.h"

//------------------------------------------------------------------------------
// CONSTRUCTORS
//------------------------------------------------------------------------------
Spell::Spell() {}

bool Spell::operator==(const Spell &other) const {
	return
		(  this->id == other.id
		&& this->name == other.name
		&& this->book == other.book
		&& this->tech == other.tech
		&& this->level == other.level
		&& this->cost == other.cost
		&& this->text == other.text
		&& this->effects == other.effects
		&& this->counterspell_selector == other.counterspell_selector
		);
}

//------------------------------------------------------------------------------
// GETTERS
//------------------------------------------------------------------------------
std::string Spell::get_id() const { return id; }
std::string Spell::get_name() const { return name; }
std::string Spell::get_book() const { return book; }
int Spell::get_tech() const { return tech; }
int Spell::get_level() const { return level; }
int Spell::get_cost() const { return cost; }
std::string Spell::get_text() const { return text; }
const std::vector<Effect> &Spell::get_effects() const { return effects; }
bool Spell::is_counterspell() const { return counterspell_selector.is_valid(); }
const Selector &Spell::get_counterspell_selector() const {
	return counterspell_selector;
}
bool Spell::is_tech_spell() const { return tech_spell; }

//------------------------------------------------------------------------------
// SERIALIZATION
//------------------------------------------------------------------------------
void from_json(const json &j, Spell &spell) {
	spell.id = j.at("id");
	spell.name = j.at("name");
	spell.book = j.at("book");
	spell.tech = j.at("tech");
	spell.level = j.at("level");
	spell.cost = j.at("cost");
	spell.text = j.at("text");
	spell.tech_spell = false;
	for (const auto &it : j.at("effects")) {
		Effect e = it;
		if (e.get_effect_type() == ET_TECH) {
			spell.tech_spell = true;
		}
		spell.effects.push_back(e);
	}
	if (j.find("counterspell") != j.end()) {
		spell.counterspell_selector = j.at("counterspell");
	}
}

void to_json(json &j, const Spell &spell) {
	j["id"] = spell.id;
	j["name"] = spell.name;
	j["book"] = spell.book;
	j["tech"] = spell.tech;
	j["level"] = spell.level;
	j["cost"] = spell.cost;
	j["text"] = spell.text;
	j["effects"] = spell.effects;
	if (spell.is_counterspell()) {
		j["counterspell"] = spell.counterspell_selector;
	}
}
