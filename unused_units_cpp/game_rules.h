/* A class for holding game "rules" - but really only holds unitkinds. */


#pragma once

#include <map>

#include "unit_kind.h"
#include "json.h"

using json = nlohmann::json;


class GameRules {
	public:
		GameRules();
		// Initialize rules from a rules json file (json list of units)
		GameRules(std::string rules_filename);
		const UnitKind &get_unit_kind(std::string id) const;
		int get_unit_cap() const;
		const std::map<std::string, UnitKind> &get_unit_kinds() const;
		// Get unit kinds for starting units. Duplicate kinds represent multiple
		// starting units of a single kind.
		const std::vector<UnitKind*> &get_starting_units() const;

		friend void to_json(json &j, const GameRules &rules);
		friend void from_json(const json &j, GameRules &rules);

	private:
		std::map<std::string, UnitKind> unit_kinds;
		int unit_cap;
		std::vector<UnitKind*> starting_units;
};
