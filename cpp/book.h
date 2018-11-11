/* A Book is a collection of Spells. It does not contain the Spell instances
 * themselves, but instead contains ids by which the spells can be looked up in
 * the GameRules. */

#pragma once

#include <vector>

#include "json.h"

using json = nlohmann::json;

class Book {
	public:
		Book();
		std::string get_id() const;
		std::string get_name() const;
		const std::vector<std::string> get_spells() const;

		friend void to_json(json &j, const Book &rules);
		friend void from_json(const json &j, Book &rules);

	private:
		std::string id;
		std::string name;
		std::vector<std::string> spells;
};
