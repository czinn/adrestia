#include "game_rules.h"

#include <fstream>

//------------------------------------------------------------------------------
// C++ SEMANTICS
//------------------------------------------------------------------------------
GameRules::GameRules() {}
GameRules::GameRules(std::string rules_filename) {
	json j;
	std::ifstream in(rules_filename);
	in >> j;
	*this = j;
}

bool GameRules::operator==(const GameRules &other) const {
	return (
		this->stickies == other.stickies &&
		this->spells == other.spells &&
		this->books == other.books &&
		this->mana_cap == other.mana_cap &&
		this->initial_health == other.initial_health &&
		this->initial_mana_regen == other.initial_mana_regen &&
		this->spell_cap == other.spell_cap
	);
}

//------------------------------------------------------------------------------
// GETTERS
//------------------------------------------------------------------------------
const Sticky &GameRules::get_sticky(std::string id) const { return stickies.at(id); }
const Spell &GameRules::get_spell(std::string id) const { return spells.at(id); }
const Book &GameRules::get_book(std::string id) const { return books.at(id); }
const std::map<std::string, Book> &GameRules::get_books() const { return books; }
int GameRules::get_mana_cap() const { return mana_cap; }
int GameRules::get_initial_health() const { return initial_health; }
int GameRules::get_initial_mana_regen() const { return initial_mana_regen; }
int GameRules::get_spell_cap() const { return spell_cap; }

//------------------------------------------------------------------------------
// SERIALIZATION
//------------------------------------------------------------------------------
void from_json(const json &j, GameRules &rules) {
	for (const auto &it : j.at("stickies")) {
		rules.stickies.emplace(it["id"].get<std::string>(), it);
	}
	for (const auto &it : j.at("spells")) {
		rules.spells.emplace(it["id"].get<std::string>(), it);
	}
	for (const auto &it : j.at("books")) {
		rules.books.emplace(it["id"].get<std::string>(), it);
	}
	rules.mana_cap = j.at("mana_cap");
	rules.initial_health = j.at("initial_health");
	rules.initial_mana_regen = j.at("initial_mana_regen");
	rules.spell_cap = j.at("spell_cap");
}

void to_json(json &j, const GameRules &rules) {
	for (const auto &[k, v] : rules.stickies) {
		j["stickies"].push_back(v);
	}
	for (const auto &[k, v] : rules.spells) {
		j["spells"].push_back(v);
	}
	for (const auto &[k, v] : rules.books) {
		j["books"].push_back(v);
	}
	j["mana_cap"] = rules.mana_cap;
	j["initial_health"] = rules.initial_health;
	j["initial_mana_regen"] = rules.initial_mana_regen;
	j["spell_cap"] = rules.spell_cap;
}
