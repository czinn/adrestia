/* Rules for the games, including all spells and books, as well as initial
 * health / mana generation. */

#pragma once

#include <map>

#include "spell.h"
#include "book.h"
#include "json.h"

using json = nlohmann::json;


class GameRules {
	public:
		GameRules();
		// Initializes rules from json file.
		GameRules(std::string rules_filename);
		const Spell &get_spell(std::string id) const;
		const Book &get_book(std::string id) const;
		const std::map<std::string, Book> &get_books() const;
		int get_mana_cap() const;
		int get_initial_health() const;
		int get_initial_mana_regen() const;

		friend void from_json(const json &j, GameRules &rules);
		friend void to_json(json &j, const GameRules &rules);

	private:
		// Note that there is no way to access the list of spells directly. If you
		// want to do this, you probably want to get the list of books and iterate
		// through that instead.
    std::map<std::string, Spell> spells;
		std::map<std::string, Book> books;
		int mana_cap;
		int initial_health;
		int initial_mana_regen;
};
