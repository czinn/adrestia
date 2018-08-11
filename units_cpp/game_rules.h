/* A class for holding game "rules" - but really only holds unitkinds. */


#pragma once

#include <map>

#include "unit_kind.h"
#include "json.h"

using json = nlohmann::json;


class GameRules {
	public:
		GameRules();
		GameRules(std::string rules_filename);  // Initialize rules from a rules json file (json list of units)
		const UnitKind &get_unit_kind(std::string id) const;  // Return unit kind for this id
		int get_unit_cap() const;
		const std::map<std::string, UnitKind> &get_unit_kinds() const;  // Get all unit kinds
		const std::vector<UnitKind*> &get_starting_units() const;  // Get unit kinds for starting units

		friend void to_json(json &j, const GameRules &rules);
		friend void from_json(const json &j, GameRules &rules);

	private:
		std::map<std::string, UnitKind> unit_kinds;
		int unit_cap;
		std::vector<UnitKind*> starting_units;
};
