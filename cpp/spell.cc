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

//------------------------------------------------------------------------------
// SERIALIZATION
//------------------------------------------------------------------------------
void from_json(const json &j, Spell &spell) {
	spell.id = j["id"];
	spell.name = j["name"];
}

void to_json(json &j, const Spell &spell) {
	j["id"] = spell.id;
	j["name"] = spell.name;
}
