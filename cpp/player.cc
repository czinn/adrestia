#include "player.h"

//------------------------------------------------------------------------------
// C++ SEMANTICS
//------------------------------------------------------------------------------
Player::Player(const GameRules &rules, const std::vector<std::string> books) {
	this->max_hp = rules.get_initial_health();
	this->hp = this->max_hp;
	this->mp = 0;
	this->mp_regen = rules.get_initial_mana_regen();
	for (const auto &book_id : books) {
		this->books.push_back(&rules.get_book(book_id));
		this->tech.push_back(0);
	}
}

bool Player::operator==(const Player &other) const {
	return this == &other;
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
	for (auto *s : player.outbound_stickies) {
		j["outbound_stickies"].push_back(*s);
	}
	for (auto *s : player.inbound_stickies) {
		j["inbound_stickies"].push_back(*s);
	}
}
