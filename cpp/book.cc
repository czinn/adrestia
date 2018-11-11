#include "book.h"

//------------------------------------------------------------------------------
// CONSTRUCTORS
//------------------------------------------------------------------------------
Book::Book() {}

//------------------------------------------------------------------------------
// GETTERS
//------------------------------------------------------------------------------
std::string Book::get_id() const { return id; }
std::string Book::get_name() const { return name; }
const std::vector<std::string> Book::get_spells() const { return spells; }

//------------------------------------------------------------------------------
// SERIALIZATION
//------------------------------------------------------------------------------
void from_json(const json &j, Book &book) {
	book.id = j.at("id");
	book.name = j.at("name");
	for (auto it = j.at("spells").begin(), end = j.at("spells").end(); it != end; it++) {
		book.spells.push_back(*it);
	}
}

void to_json(json &j, const Book &book) {
	j["id"] = book.id;
	j["name"] = book.name;
	for (auto it = book.spells.begin(); it != book.spells.end(); it++) {
		j["spells"].push_back(*it);
	}
}
