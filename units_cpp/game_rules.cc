#include "game_rules.h"

#include <fstream>


//----------------------------------------------------------------------------------------------------------------------
// CONSTRUCTORS
//----------------------------------------------------------------------------------------------------------------------
GameRules::GameRules() {}

GameRules::GameRules(std::string rules_filename) {
	/*! \brief Create GameRules from rules json file (list of units) */

	json j;
	std::ifstream in("rules.json");
	in >> j;
	*this = j;
}


//----------------------------------------------------------------------------------------------------------------------
// GETTERS AND SETTERS
//----------------------------------------------------------------------------------------------------------------------
const UnitKind &GameRules::get_unit_kind(std::string id) const {
	return unit_kinds.at(id);
}


int GameRules::get_unit_cap() const {
	return unit_cap;
}


const std::map<std::string, UnitKind> &GameRules::get_unit_kinds() const {
	return unit_kinds;
}


const std::vector<UnitKind*> &GameRules::get_starting_units() const {
	return starting_units;
}


//----------------------------------------------------------------------------------------------------------------------
// OTHER METHODS
//----------------------------------------------------------------------------------------------------------------------
void to_json(json &j, const GameRules &rules) {
	for (auto it = rules.unit_kinds.begin(); it != rules.unit_kinds.end(); it++) {
		j["unit_kinds"].push_back(it->second);
	}
	j["unit_cap"] = rules.unit_cap;
	for (auto it = rules.starting_units.begin(); it != rules.starting_units.end(); it++) {
		j["starting_units"].push_back((*it)->get_id());
	}
}


void from_json(const json &j, GameRules &rules) {
	for (auto it = j["unit_kinds"].begin(), end = j["unit_kinds"].end(); it != end; it++) {
		rules.unit_kinds.emplace((*it)["id"].get<std::string>(), *it);
	}
	rules.unit_cap = j["unit_cap"];
	rules.starting_units = std::vector<UnitKind*>();
	for (auto it = j["starting_units"].begin(), end = j["starting_units"].end(); it != end; it++) {
		rules.starting_units.push_back(&rules.unit_kinds.at(*it));
	}
}