#include "spell.h"

//------------------------------------------------------------------------------
// CONSTRUCTORS
//------------------------------------------------------------------------------
Spell::Spell() {}

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
const std::vector<Effect> Spell::get_effects() const { return effects; }

//------------------------------------------------------------------------------
// SERIALIZATION
//------------------------------------------------------------------------------
void from_json(const json &j, Spell &spell) {
	spell.id = j["id"];
	spell.name = j["name"];
	spell.book = j["book"];
	spell.tech = j["tech"];
	spell.level = j["level"];
	spell.cost = j["cost"];
	spell.text = j["text"];
	for (auto it = j["effects"].begin(), end = j["effects"].end(); it != end; it++) {
		spell.effects.push_back(*it);
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
	for (auto it = spell.effects.begin(); it != spell.effects.end(); it++) {
		j["effects"].push_back(*it);
	}
}
