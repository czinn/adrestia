#include "game_rules.h"

#include <fstream>

//------------------------------------------------------------------------------
// CONSTRUCTORS
//------------------------------------------------------------------------------
GameRules::GameRules() {}
GameRules::GameRules(std::string rules_filename) {
	json j;
	std::ifstream in(rules_filename);
	in >> j;
	*this = j;
}

//------------------------------------------------------------------------------
// GETTERS
//------------------------------------------------------------------------------
const Spell &GameRules::get_spell(std::string id) const { return spells.at(id); }
const Book &GameRules::get_book(std::string id) const { return books.at(id); }
const std::map<std::string, Book> &GameRules::get_books() const { return books; }
int GameRules::get_mana_cap() const { return mana_cap; }
int GameRules::get_initial_health() const { return initial_health; }
int GameRules::get_initial_mana_regen() const { return initial_mana_regen; }

//------------------------------------------------------------------------------
// SERIALIZATION
//------------------------------------------------------------------------------
void to_json(json &j, const GameRules &rules) {
	for (auto it = rules.spells.begin(); it != rules.spells.end(); it++) {
		j["spells"].push_back(it->second);
	}
	for (auto it = rules.books.begin(); it != rules.books.end(); it++) {
		j["books"].push_back(it->second);
	}
	j["mana_cap"] = rules.mana_cap;
	j["initial_health"] = rules.initial_health;
	j["initial_mana_regen"] = rules.initial_mana_regen;
}

void from_json(const json &j, GameRules &rules) {
	for (auto it = j["spells"].begin(), end = j["spells"].end(); it != end; it++) {
		rules.spells.emplace((*it)["id"].get<std::string>(), *it);
	}
	for (auto it = j["books"].begin(), end = j["books"].end(); it != end; it++) {
		rules.books.emplace((*it)["id"].get<std::string>(), *it);
	}
	rules.mana_cap = j["mana_cap"];
	rules.initial_health = j["initial_health"];
	rules.initial_mana_regen = j["initial_mana_regen"];
}
