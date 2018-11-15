#include "player.h"

#include "effect_instance.h"
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

size_t Player::find_book_idx(const std::string &book_id) const {
	for (size_t book_idx = 0; book_idx < books.size(); book_idx++) {
		if (books[book_idx]->get_id() == book_id) {
			return book_idx;
		}
	}
	return size_t(-1);
}

std::vector<EffectInstance> Player::pipe_effect(size_t player_id, EffectInstance &effect,
		bool inbound) {
	std::vector<EffectInstance> generated_effects;
	for (auto &sticky : stickies) {
		if (sticky.sticky.triggers_for_effect(effect, inbound)) {
			std::vector<EffectInstance> new_effects = sticky.apply(player_id, effect);
			std::copy(new_effects.begin(), new_effects.end(),
					std::back_inserter(generated_effects));
		}
	}
	return generated_effects;
}

std::vector<EffectInstance> Player::pipe_spell(size_t player_id, const Spell &spell) {
	std::vector<EffectInstance> generated_effects;
	for (auto &sticky : stickies) {
		if (sticky.sticky.triggers_for_spell(spell)) {
			std::vector<EffectInstance> new_effects = sticky.apply(player_id, spell);
			std::copy(new_effects.begin(), new_effects.end(),
					std::back_inserter(generated_effects));
		}
	}
	return generated_effects;
}

std::vector<EffectInstance> Player::pipe_turn(size_t player_id) {
	std::vector<EffectInstance> generated_effects;
	for (auto &sticky : stickies) {
		if (sticky.sticky.triggers_at_end_of_turn()) {
			std::vector<EffectInstance> new_effects = sticky.apply(player_id);
			std::copy(new_effects.begin(), new_effects.end(),
					std::back_inserter(generated_effects));
		}
	}
	return generated_effects;
}

void Player::subtract_step() {
  auto sticky = stickies.begin();
  while (sticky != stickies.end()) {
    sticky->remaining_duration.subtract_step();
    if (!sticky->remaining_duration.is_active()) {
      sticky = stickies.erase(sticky);
    } else {
      sticky++;
    }
  }
}

void Player::subtract_turn() {
  auto sticky = stickies.begin();
  while (sticky != stickies.end()) {
    sticky->remaining_duration.subtract_turn();
    if (!sticky->remaining_duration.is_active()) {
      sticky = stickies.erase(sticky);
    } else {
      sticky++;
    }
  }
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
