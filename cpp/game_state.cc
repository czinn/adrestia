#include "game_state.h"
#include "effect_instance.h"
#include "spell.h"
#include "game_view.h"

#include <deque>
#include <ostream>
#include <iostream>

//------------------------------------------------------------------------------
// C++ SEMANTICS
//------------------------------------------------------------------------------
GameState::GameState(
		const GameRules &rules,
		const std::vector<std::vector<std::string>> &player_books)
		: rules(rules) {
	size_t player_id = 0;
	for (const auto &p : player_books) {
		players.emplace_back(rules, player_id, p);
		player_id++;
	}
}

GameState::GameState(const GameRules &rules, const json &j)
	: rules(rules) {
	for (const auto &it : j.at("players")) {
		players.push_back(Player(rules, it));
	}
	for (const auto &it : j.at("history")) {
		history.push_back(it);
	}
}

GameState::GameState(
		const GameView &view,
		const std::vector<int> &tech,
		const std::vector<const Book*> &books)
	: history(view.history)
	, players(view.players)
	, rules(view.rules) {
	// TODO: charles: Fill in the missing parts of the history if required.
	players[1 - view.view_player_id].tech = tech;
	players[1 - view.view_player_id].books = books;
}

bool GameState::operator==(const GameState &other) const {
	return (
			this->players == other.players &&
			this->history == other.history
			);
}

std::ostream &operator<<(std::ostream &os, const GameState &state) {
	os << "Turn " << state.turn_number() << std::endl;
	for (size_t player_id = 0; player_id < state.players.size(); player_id++) {
		const auto &player = state.players[player_id];
		os << "Player " << player_id;
		os << " (hp: " << player.hp << "/" << player.max_hp << ")";
		os << " (mp: " << player.mp << " (+" << player.mp_regen << "))";
		os << " (books:";
		for (size_t i = 0; i < player.books.size(); i++) {
			const auto tech = player.tech[i];
			const auto *book = player.books[i];
			os << " (" << book->get_name() << ": " << tech << ")";
		}
		os << ")";
		os << " (stickies: " << player.stickies.size() << ")";
		os << std::endl;
	}
	return os;
}

//------------------------------------------------------------------------------
// BUSINESS LOGIC
//------------------------------------------------------------------------------
void append_to_effect_queue(
		std::deque<EffectInstance> *effect_queue,
		const EffectInstance &effect_instance) {
	if (effect_instance.targets_self) {
		effect_queue->push_front(effect_instance);
	} else {
		effect_queue->push_back(effect_instance);
	}
}

void append_to_effect_queue(
		std::deque<EffectInstance> *effect_queue,
		std::vector<EffectInstance> &effects) {
	for (const auto &effect_instance : effects) {
		append_to_effect_queue(effect_queue, effect_instance);
	}
}

template<bool emit_events>
void _process_effect_queue(
		GameState &state,
		std::deque<EffectInstance> *effect_queue,
		std::deque<EffectInstance> *next_effect_queue,
		std::vector<json> &events_out)
{
	while (effect_queue->size() != 0) {
		for (auto &effect_instance : *effect_queue) {
			size_t target_player_id = effect_instance.target_player;
			auto &target = state.players[target_player_id];
			std::vector<EffectInstance> generated_effects =
				emit_events ?
				target.pipe_effect(effect_instance, true, events_out) :
				target.pipe_effect(effect_instance, true);
			for (const auto &e : generated_effects) {
				append_to_effect_queue(next_effect_queue, e);
			}
			if (!effect_instance.fizzles()) {
				effect_instance.apply(state.rules, target);
				if (emit_events) {
					events_out.emplace_back(json{
						{"type", "effect"},
						{"effect", effect_instance}
					});
				}
			}
		}
		std::swap(effect_queue, next_effect_queue);
		next_effect_queue->clear();
		if (emit_events) {
			events_out.emplace_back(json{{"type", "time_point"}, {"point", "effect_queue_cleared"}});
		}
	}
}

template<bool emit_events>
bool _simulate(
	GameState &state,
	const std::vector<GameAction> actions,
	std::vector<json> &events_out)
{
	auto &players = state.players;

	if (state.winners().size() > 0) {
		return false;
	}
	if (actions.size() != players.size()) {
		return false;
	}
	for (size_t player_id = 0; player_id < players.size(); player_id++) {
		if (!state.is_valid_action(player_id, actions[player_id])) {
			std::cout << "player " << player_id << " did a bad" << std::endl;
			return false;
		}
	}

	// history has its eyes on you
	state.history.push_back(actions);

	size_t max_action_length = 0;
	for (const auto &action : actions) {
		max_action_length = std::max(max_action_length, action.size());
	}

	// Cast spells.
	std::deque<EffectInstance> queue1;
	std::deque<EffectInstance> queue2;
	std::deque<EffectInstance> *effect_queue = &queue1;
	std::deque<EffectInstance> *next_effect_queue = &queue2;
	for (size_t spell_idx = 0; spell_idx < max_action_length; spell_idx++) {
		std::vector<const Spell *> spells_in_flight(players.size(), nullptr);

		// Tick down stickies that last for some number of steps.
		for (size_t player_id = 0; player_id < players.size(); player_id++) {
			auto &player = players[player_id];
			emit_events ? player.subtract_step(events_out) : player.subtract_step();
		}

		if (emit_events) {
			events_out.emplace_back(json{{"type", "time_point"}, {"point", "step_stickies_ticked"}});
		}
		
		// Fire spells, putting them in flight.
		for (size_t player_id = 0; player_id < players.size(); player_id++) {
			auto &caster = players[player_id];

			if (spell_idx >= actions[player_id].size()) continue;

			const auto &spell = state.rules.get_spell(actions[player_id][spell_idx]);
			if (caster.mp >= spell.get_cost()) {
				if (emit_events) {
					events_out.emplace_back(json{
						{"type", "fire_spell"},
						{"player", player_id},
						{"spell", spell.get_id()},
						{"index", spell_idx},
						{"success", true}
					});
				}
				caster.mp -= spell.get_cost();
				spells_in_flight[player_id] = &spell;
				std::vector<EffectInstance> generated_effects =
					emit_events ?
					caster.pipe_spell(spell, events_out) :
					caster.pipe_spell(spell);
				append_to_effect_queue(effect_queue, generated_effects);
			} else {
				if (emit_events) {
					events_out.emplace_back(json{
						{"type", "fire_spell"},
						{"player", player_id},
						{"spell", spell.get_id()},
						{"index", spell_idx},
						{"success", false}
					});
				}
			}
		}

		if (emit_events) {
			events_out.emplace_back(json{{"type", "time_point"}, {"point", "spells_fired"}});
		}

		// Process effects triggered by spells.
		_process_effect_queue<emit_events>(state, effect_queue, next_effect_queue, events_out);

		// Resolve effects of in-flight spells.
		for (size_t player_id = 0; player_id < players.size(); player_id++) {
			auto &caster = players[player_id];
			const auto *spell_ptr = spells_in_flight[player_id];
			if (spell_ptr == nullptr) continue;
			const auto &spell = *spell_ptr;

			// Check for counterspells.
			if (spell_idx < actions[1 - player_id].size()) {
				const auto &other_spell = state.rules.get_spell(actions[1 - player_id][spell_idx]);
				if (!spell.is_tech_spell() &&
						other_spell.is_counterspell() &&
						other_spell.get_counterspell_selector().selects_spell(spell)) {
					if (emit_events) {
						events_out.emplace_back(json{
							{"type", "spell_countered"},
							{"index", spell_idx},
							{"player", player_id},
						});
					}
					continue;
				}
			}

			for (const auto &effect : spell.get_effects()) {
				EffectInstance effect_instance(player_id, spell, effect);
				std::vector<EffectInstance> generated_effects =
					emit_events ?
					caster.pipe_effect(effect_instance, false, events_out) :
					caster.pipe_effect(effect_instance, false);
				append_to_effect_queue(next_effect_queue, generated_effects);
				append_to_effect_queue(effect_queue, effect_instance);
			}
			events_out.emplace_back(json{
				{"type", "spell_hit"},
				{"index", spell_idx},
				{"caster", player_id},
			});
		}

		if (emit_events) {
			events_out.emplace_back(json{{"type", "time_point"}, {"point", "spells_hit_or_countered"}});
		}

		// Process effects created by spells.
		_process_effect_queue<emit_events>(state, effect_queue, next_effect_queue, events_out);
	}

	for (size_t player_id = 0; player_id < players.size(); player_id++) {
		std::vector<EffectInstance> generated_effects =
			emit_events ?
			players[player_id].pipe_turn(events_out) :
			players[player_id].pipe_turn();
		append_to_effect_queue(effect_queue, generated_effects);
	}

	// Process effects created by god.
	_process_effect_queue<emit_events>(state, effect_queue, next_effect_queue, events_out);

	for (size_t player_id = 0; player_id < players.size(); player_id++) {
		auto &player = players[player_id];
		emit_events ? player.subtract_turn(events_out) : player.subtract_turn();
		int mp_gain = std::min(state.rules.get_mana_cap() - player.mp, player.mp_regen);
		if (emit_events) {
			events_out.emplace_back(json{
				{"type", "player_mp"},
				{"player", player_id},
				{"amount", mp_gain},
			});
		}
		player.mp += mp_gain;
	}

	return true;
}

bool GameState::simulate(const std::vector<GameAction> &actions) {
	std::vector<json> unused;
	return _simulate<false>(*this, actions, unused);
}

bool GameState::simulate(const std::vector<GameAction> &actions, std::vector<json> &events_out) {
	return _simulate<true>(*this, actions, events_out);
}

void GameState::apply_event(const json &event) {
	std::string type = event.at("type").get<std::string>();
	if (type == "player_mp") {
		size_t player_id = event.at("player").get<size_t>();
		int amount = event.at("amount").get<int>();
		players[player_id].mp += amount;
	} else if (type == "effect") {
		const json &effect = event.at("effect");
		Player &player = players[effect.at("target_player").get<size_t>()];
		switch (effect.at("kind").get<EffectKind>()) {
			case EK_TECH:
				{
					auto [spell, book_idx] = player.find_spell(effect.at("spell_id"));
					if (spell != nullptr) {
						player.tech[book_idx] += effect.at("amount").get<int>();
					}
				}
				break;
			case EK_HEALTH:
				player.hp += effect.at("amount").get<int>();
				player.hp = std::min(player.hp, player.max_hp);
				break;
			case EK_MANA:
				player.mp += effect.at("amount").get<int>();
				player.mp = std::max(0, player.mp);
				player.mp = std::min(rules.get_mana_cap(), player.mp);
				break;
			case EK_REGEN:
				player.mp_regen += effect.at("amount").get<int>();
				player.mp_regen = std::max(0, player.mp_regen);
				break;
			case EK_STICKY:
				{
					StickyInvoker sticky_invoker = effect.at("sticky");
					const Spell &spell = rules.get_spell(effect.at("spell_id"));
					player.add_sticky(
							StickyInstance(
								spell,
								rules.get_sticky(sticky_invoker.get_sticky_id()),
								sticky_invoker
								));
				}
				break;
		}
	} else if (type == "fire_spell") {
		if (event.at("success").get<bool>()) {
			size_t player_id = event.at("player").get<size_t>();
			Player &player = players[player_id];
			const Spell &spell = rules.get_spell(event.at("spell"));
			player.mp -= spell.get_cost();
			// We don't pipe the spell here, because piping the spell generates its
			// own effects, which we want to display separately.
		}
	} else if (type == "spell_countered") {
		// State is unchanged, this event is used only for animation.
	} else if (type == "spell_hit") {
		// State is unchanged, this event is used only for animation.
		// We don't generate effects here because they're piped through stickies
		// and applied to players in separate events.
	} else if (type == "sticky_activated") {
		// State is unchanged, this event is used only for animation.
	} else if (type == "sticky_amount_changed"
			|| type == "sticky_expired"
			|| type == "sticky_duration_changed") {
		size_t player_id = event.at("player").get<size_t>();
		size_t sticky_index = event.at("sticky_index").get<size_t>();
		auto it = players[player_id].stickies.begin();
		for (size_t i = 0; i < sticky_index; i++, it++);
		if (type == "sticky_amount_changed") {
			it->amount = event.at("amount").get<int>();
		} else if (type == "sticky_duration_changed") {
			it->remaining_duration = event.at("duration");
		} else if (type == "sticky_expired") {
			players[player_id].stickies.erase(it);
		}
	} else if (type == "time_point") {
		// State is unchanged, this event is used only for animation.
	} else {
		// We should be handling all event types.
		std::cout << type << std::endl;
		assert(false);
	}
}

bool GameState::is_valid_action(size_t player_id, GameAction action) const {
	// TODO: Return code or list of codes for why action isn't valid.
	if (action.size() > rules.get_spell_cap()) {
		std::cout << "too many actions" << std::endl;
		return false;
	}
	const Player &player = players[player_id];
	int mp_left = player.mp;
	// After the player has a cast a tech spell this turn, this is set to the
	// tech they increased. This prevents the player from casting multiple tech
	// spells in a turn, and allows them to use the tech immediately.
	int turn_tech = -1;
	for (const auto &spell_id : action) {
		auto [spell, book_idx] = player.find_spell(spell_id);
		if (spell == nullptr) {
			std::cout << "spell doesn't exist" << std::endl;
			return false;
		}
		if (player.tech[book_idx] + (turn_tech == book_idx ? 1 : 0) < spell->get_tech()) {
			std::cout << "not enough tech" << std::endl;
			return false;
		}
		if (spell->is_tech_spell() && turn_tech != -1) {
			std::cout << "already cast tech" << std::endl;
			return false;
		}
		if (player.level() + (turn_tech != -1 ? 1 : 0) < spell->get_level()) {
			std::cout << "not enough level" << std::endl;
			return false;
		}
		mp_left -= spell->get_cost();
		if (mp_left < 0) {
			std::cout << "not enough mana" << std::endl;
			return false;
		}
		if (spell->is_tech_spell()) {
			turn_tech = book_idx;
		}
	}
	return true;
}

int GameState::turn_number() const {
	return history.size() + 1;
}

std::vector<size_t> GameState::winners() const {
	std::vector<size_t> alive;
	for (size_t i = 0; i < players.size(); i++) {
		if (players[i].hp > 0) alive.push_back(i);
	}
	if (alive.size() == 1) return alive;
	if (alive.size() == 0) {
		for (size_t i = 0; i < players.size(); i++) {
			alive.push_back(i);
		}
		return alive;
	}
	return {};
}

//------------------------------------------------------------------------------
// SERIALIZATION
//------------------------------------------------------------------------------
void to_json(json &j, const GameState &state) {
	j["players"] = state.players;
	j["history"] = state.history;
}
