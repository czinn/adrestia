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
	this->mp_regen = rules.get_initial_mana_regen();
	this->hp = this->max_hp;
	this->mp = this->mp_regen;
	for (const auto &book_id : books) {
		this->books.push_back(&rules.get_book(book_id));
		this->tech.push_back(0);
	}
}

Player::Player(const GameRules &rules, const json &j) 
	: hp(j.at("hp"))
	, max_hp(j.at("max_hp"))
	, mp(j.at("mp"))
	, mp_regen(j.at("mp_regen"))
	, rules(rules) {
	for (const auto &it : j.at("tech")) {
		tech.push_back(it);
	}
	for (const auto &it : j.at("books")) {
		books.push_back(&rules.get_book(it.get<std::string>()));
	}
	for (const auto &it : j.at("stickies")) {
		stickies.push_back(StickyInstance(rules, it));
	}
}

bool Player::operator==(const Player &other) const {
	return (
			this->hp == other.hp &&
			this->max_hp == other.max_hp &&
			this->mp == other.mp &&
			this->mp_regen == other.mp_regen &&
			this->tech == other.tech &&
			this->books == other.books &&
			this->stickies == other.stickies
			);
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

template<bool emit_events>
std::vector<EffectInstance> _pipe_effect(
		Player &p,
		size_t player_id,
		EffectInstance &effect,
		bool inbound,
		std::vector<json> &events_out) {
	std::vector<EffectInstance> generated_effects;
	for (auto &sticky : p.stickies) {
		if (sticky.sticky.triggers_for_effect(effect, inbound)) {
			std::vector<EffectInstance> new_effects =
				emit_events ?
				sticky.apply_to_effect(player_id, effect, events_out) :
				sticky.apply_to_effect(player_id, effect);
			std::copy(new_effects.begin(), new_effects.end(),
					std::back_inserter(generated_effects));
		}
	}
	return generated_effects;
}

std::vector<EffectInstance> Player::pipe_effect(
		size_t player_id,
		EffectInstance &effect,
		bool inbound) {
	std::vector<json> unused;
	return _pipe_effect<false>(*this, player_id, effect, inbound, unused);
}

std::vector<EffectInstance> Player::pipe_effect(
		size_t player_id,
		EffectInstance &effect,
		bool inbound,
		std::vector<json> &events_out) {
	return _pipe_effect<true>(*this, player_id, effect, inbound, events_out);
}

template<bool emit_events>
std::vector<EffectInstance> _pipe_spell(
		Player &p,
		size_t player_id,
		const Spell &spell,
		std::vector<json> &events_out) {
	std::vector<EffectInstance> generated_effects;
	for (auto &sticky : p.stickies) {
		if (sticky.sticky.triggers_for_spell(spell)) {
			std::vector<EffectInstance> new_effects =
				emit_events ?
				sticky.apply_to_spell(player_id, spell, events_out) :
				sticky.apply_to_spell(player_id, spell);
			std::copy(new_effects.begin(), new_effects.end(),
					std::back_inserter(generated_effects));
		}
	}
	return generated_effects;
}

std::vector<EffectInstance> Player::pipe_spell(
		size_t player_id,
		const Spell &spell) {
	std::vector<json> unused;
	return _pipe_spell<false>(*this, player_id, spell, unused);
}

std::vector<EffectInstance> Player::pipe_spell(
		size_t player_id,
		const Spell &spell,
		std::vector<json> &events_out) {
	return _pipe_spell<true>(*this, player_id, spell, events_out);
}

template<bool emit_events>
std::vector<EffectInstance> _pipe_turn(
		Player &p,
		size_t player_id,
		std::vector<json> &events_out) {
	std::vector<EffectInstance> generated_effects;
	for (auto &sticky : p.stickies) {
		if (sticky.sticky.triggers_at_end_of_turn()) {
			std::vector<EffectInstance> new_effects =
				emit_events ?
				sticky.apply_to_turn(player_id, events_out) :
				sticky.apply_to_turn(player_id);
			std::copy(new_effects.begin(), new_effects.end(),
					std::back_inserter(generated_effects));
		}
	}
	return generated_effects;
}

std::vector<EffectInstance> Player::pipe_turn(
		size_t player_id) {
	std::vector<json> unused;
	return _pipe_turn<false>(*this, player_id, unused);
}

std::vector<EffectInstance> Player::pipe_turn(
		size_t player_id,
		std::vector<json> &events_out) {
	return _pipe_turn<true>(*this, player_id, events_out);
}

template<bool emit_events>
void _subtract_step(Player &p, std::vector<json> &events_out) {
	auto sticky = p.stickies.begin();
	while (sticky != p.stickies.end()) {
		sticky->remaining_duration.subtract_step();
		if (!sticky->remaining_duration.is_active()) {
			sticky = p.stickies.erase(sticky);
		} else {
			sticky++;
		}
	}
}

void Player::subtract_step() {
	std::vector<json> unused;
	return _subtract_step<false>(*this, unused);
}

void Player::subtract_step(std::vector<json> &events_out) {
	return _subtract_step<true>(*this, events_out);
}

template<bool emit_events>
void _subtract_turn(Player &p, std::vector<json> &events_out) {
	auto sticky = p.stickies.begin();
	while (sticky != p.stickies.end()) {
		sticky->remaining_duration.subtract_turn();
		if (!sticky->remaining_duration.is_active()) {
			sticky = p.stickies.erase(sticky);
		} else {
			sticky++;
		}
	}
}

void Player::subtract_turn() {
	std::vector<json> unused;
	return _subtract_turn<false>(*this, unused);
}

void Player::subtract_turn(std::vector<json> &events_out) {
	return _subtract_turn<true>(*this, events_out);
}

//------------------------------------------------------------------------------
// SERIALIZATION
//------------------------------------------------------------------------------
void to_json(json &j, const Player &player) {
	j["hp"] = player.hp;
	j["max_hp"] = player.max_hp;
	j["mp"] = player.mp;
	j["mp_regen"] = player.mp_regen;
	j["tech"] = player.tech;
	for (auto *b : player.books) {
		j["books"].push_back(b->get_id());
	}
	j["stickies"] = player.stickies;
}
