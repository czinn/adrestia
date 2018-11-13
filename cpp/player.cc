#include "player.h"

#include "util.h"

//------------------------------------------------------------------------------
// C++ SEMANTICS
//------------------------------------------------------------------------------
Player::Player(const GameRules &rules, const std::vector<std::string> books)
	: rules(rules)
{
	this->max_hp = rules.get_initial_health();
	this->hp = this->max_hp;
	this->mp_regen = rules.get_initial_mana_regen();
	this->mp = this->mp_regen;
	for (const auto &book_id : books) {
		this->books.push_back(&rules.get_book(book_id));
		this->tech.push_back(0);
	}
}

bool Player::operator==(const Player &other) const {
	return this == &other;
}

//------------------------------------------------------------------------------
// BUSINESS LOGIC
//------------------------------------------------------------------------------
std::pair<const Spell*, size_t> Player::find_spell(const std::string &spell_id) const {
	for (size_t book_idx = 0; book_idx < books.size(); book_idx++) {
		const Book *book = books[book_idx];
		if (contains(book->get_spells(), spell_id)) {
			return std::make_pair(&rules.get_spell(spell_id), book_idx);
		}
	}
	return std::make_pair(nullptr, size_t(-1));
}

int Player::level() const {
	int result = 0;
	for (int x : tech) result += x;
	return result;
}

//------------------------------------------------------------------------------
// SERIALIZATION
//------------------------------------------------------------------------------
void to_json(json &j, const Player &player) {
	j["hp"] = player.hp;
	j["max_hp"] = player.max_hp;
	j["mp"] = player.mp;
	j["mp_regen"] = player.mp_regen;
	for (auto t : player.tech) {
		j["tech"].push_back(t);
	}
	for (auto *b : player.books) {
		j["books"].push_back(b->get_id());
	}
}
