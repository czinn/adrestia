#include "player.h"

#include "effect_instance.h"
#include "util.h"

//------------------------------------------------------------------------------
// C++ SEMANTICS
//------------------------------------------------------------------------------
Player::Player(const GameRules &rules, size_t id, const std::vector<std::string> books)
	: id(id), rules(rules)
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
	: id(j.at("id"))
	, hp(j.at("hp"))
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
			this->id == other.id &&
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
void _iterate_stickies(
		Player &p,
		std::function<bool(StickyInstance &, size_t)> f,
		bool emit_duration_changes,
		std::vector<json> &events_out) {
	auto sticky = p.stickies.begin();
	size_t sticky_index = 0;
	while (sticky != p.stickies.end()) {
		Duration initial_duration = sticky->remaining_duration;
		bool keep_going = f(*sticky, sticky_index);
		if (!sticky->is_active()) {
			sticky = p.stickies.erase(sticky);
			if (emit_events) {
				events_out.emplace_back(json{
					{"type", "sticky_expired"},
					{"player", p.id},
					{"sticky_index", sticky_index}
				});
			}
			sticky_index--;
		} else {
			if (emit_events && emit_duration_changes &&
					!(sticky->remaining_duration == initial_duration)) {
				events_out.emplace_back(json{
					{"type", "sticky_duration_changed"},
					{"player", p.id},
					{"sticky_index", sticky_index},
					{"duration", sticky->remaining_duration}
				});
			}
			sticky++;
		}
		if (!keep_going) {
			break;
		}
		sticky_index++;
	}
}

template<bool emit_events>
std::vector<EffectInstance> _pipe_effect(
		Player &p,
		EffectInstance &effect,
		bool inbound,
		std::vector<json> &events_out) {
	std::vector<EffectInstance> generated_effects;
	// Handle special case for tricks_fury
	if (!inbound && effect.spell.get_id() == "tricks_fury") {
		effect.amount = -(p.rules.get_initial_health() - p.hp);
	}
	// Do normal effect handling
	_iterate_stickies<emit_events>(p, [&](auto &sticky, size_t sticky_index) {
				if (sticky.sticky.triggers_for_effect(effect, inbound)) {
					std::vector<EffectInstance> new_effects =
						emit_events ?
						sticky.apply_to_effect(p.id, effect, sticky_index, events_out) :
						sticky.apply_to_effect(p.id, effect);
					std::copy(new_effects.begin(), new_effects.end(),
							std::back_inserter(generated_effects));
					if (effect.fizzles()) {
						return false;
					}
				}
				return true;
			}, false, events_out);
	return generated_effects;
}

std::vector<EffectInstance> Player::pipe_effect(
		EffectInstance &effect,
		bool inbound) {
	std::vector<json> unused;
	return _pipe_effect<false>(*this, effect, inbound, unused);
}

std::vector<EffectInstance> Player::pipe_effect(
		EffectInstance &effect,
		bool inbound,
		std::vector<json> &events_out) {
	return _pipe_effect<true>(*this, effect, inbound, events_out);
}

template<bool emit_events>
std::vector<EffectInstance> _pipe_spell(
		Player &p,
		const Spell &spell,
		std::vector<json> &events_out) {
	std::vector<EffectInstance> generated_effects;
	_iterate_stickies<emit_events>(p, [&](auto &sticky, size_t sticky_index) {
				if (sticky.sticky.triggers_for_spell(spell)) {
					std::vector<EffectInstance> new_effects =
						emit_events ?
						sticky.apply_to_spell(p.id, spell, sticky_index, events_out) :
						sticky.apply_to_spell(p.id, spell);
					std::copy(new_effects.begin(), new_effects.end(),
							std::back_inserter(generated_effects));
				}
				return true;
			}, false, events_out);
	return generated_effects;
}

std::vector<EffectInstance> Player::pipe_spell(
		const Spell &spell) {
	std::vector<json> unused;
	return _pipe_spell<false>(*this, spell, unused);
}

std::vector<EffectInstance> Player::pipe_spell(
		const Spell &spell,
		std::vector<json> &events_out) {
	return _pipe_spell<true>(*this, spell, events_out);
}

template<bool emit_events>
std::vector<EffectInstance> _pipe_turn(
		Player &p,
		std::vector<json> &events_out) {
	std::vector<EffectInstance> generated_effects;
	_iterate_stickies<emit_events>(p, [&](auto &sticky, size_t sticky_index) {
				if (sticky.sticky.triggers_at_end_of_turn()) {
					std::vector<EffectInstance> new_effects =
						emit_events ?
						sticky.apply_to_turn(p.id, sticky_index, events_out) :
						sticky.apply_to_turn(p.id);
					std::copy(new_effects.begin(), new_effects.end(),
							std::back_inserter(generated_effects));
				}
				return true;
			}, false, events_out);
	return generated_effects;
}

std::vector<EffectInstance> Player::pipe_turn() {
	std::vector<json> unused;
	return _pipe_turn<false>(*this, unused);
}

std::vector<EffectInstance> Player::pipe_turn(
		std::vector<json> &events_out) {
	return _pipe_turn<true>(*this, events_out);
}

template<bool emit_events>
void _subtract_step(Player &p, std::vector<json> &events_out) {
	_iterate_stickies<emit_events>(p, [&](auto &sticky, size_t sticky_index) {
				Duration initial_duration = sticky.remaining_duration;
				sticky.remaining_duration.subtract_step();
				if (emit_events && sticky.remaining_duration.is_active() &&
						!(sticky.remaining_duration == initial_duration)) {
					events_out.emplace_back(json{
						{"type", "sticky_duration_changed"},
						{"player", p.id},
						{"sticky_index", sticky_index},
						{"duration", sticky.remaining_duration}
					});
				}
				return true;
			}, true, events_out);
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
	_iterate_stickies<emit_events>(p, [&](auto &sticky, size_t sticky_index) {
				Duration initial_duration = sticky.remaining_duration;
				sticky.remaining_duration.subtract_turn();
				if (emit_events && sticky.remaining_duration.is_active() &&
						!(sticky.remaining_duration == initial_duration)) {
					events_out.emplace_back(json{
						{"type", "sticky_duration_changed"},
						{"player", p.id},
						{"sticky_index", sticky_index},
						{"duration", sticky.remaining_duration}
					});
				}
				return true;
			}, true, events_out);
}

void Player::subtract_turn() {
	std::vector<json> unused;
	return _subtract_turn<false>(*this, unused);
}

void Player::subtract_turn(std::vector<json> &events_out) {
	return _subtract_turn<true>(*this, events_out);
}

void Player::add_sticky(const StickyInstance &sticky) {
	// TODO: charles: Stacking is disabled until we can emit a different event.
	// Really, I want to make a separate value called "stacks" instead of reusing
	// quantity and clean up the logic a bit.
	if (!sticky.sticky.get_stacks() || true) {
		// Just add it.
		stickies.push_back(sticky);
	} else {
		// Try to find a sticky on which this one can stack.
		for (auto &it : stickies) {
			if (it.sticky.get_id() == sticky.sticky.get_id() &&
					it.remaining_duration == sticky.remaining_duration) {
				it.amount++;
				return;
			}
		}
		// Otherwise, just add it.
		stickies.push_back(sticky);
	}
}

//------------------------------------------------------------------------------
// SERIALIZATION
//------------------------------------------------------------------------------
void to_json(json &j, const Player &player) {
	j["id"] = player.id;
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
