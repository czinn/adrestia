/* Description of a spell and all its effects. There is one instance of Spell
 * per type of spell. New instances are not created when players cast spells;
 * instead, spells are referenced by id. */

#pragma once

#include "json.h"

using json = nlohmann::json;

class Spell {
	public:
		Spell();
		std::string get_id() const;
		std::string get_name() const;

		friend void to_json(json &j, const Spell &rules);
		friend void from_json(const json &j, Spell &rules);

	private:
		std::string id;
		std::string name;
};
