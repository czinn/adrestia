#include "game_state.h"
#include "effect_instance.h"
#include "spell.h"

#include <deque>

//------------------------------------------------------------------------------
// C++ SEMANTICS
//------------------------------------------------------------------------------
GameState::GameState(
	const GameRules &rules,
	const std::vector<std::vector<std::string>> &player_books)
	: rules(rules) {
	for (const auto &p : player_books) {
		players.emplace_back(rules, p);
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

bool GameState::operator==(const GameState &other) const {
	return (
			this->players == other.players &&
			this->history == other.history
			);
}

//------------------------------------------------------------------------------
// BUSINESS LOGIC
//------------------------------------------------------------------------------
bool GameState::is_valid_action(size_t player_id, GameAction action) const {
	// TODO: Return code or list of codes for why action isn't valid.
	const Player &player = players[player_id];
	int mp_left = player.mp;
	// After the player has a cast a tech spell this turn, this is set to the
	// tech they increased. This prevents the player from casting multiple tech
	// spells in a turn, and allows them to use the tech immediately.
	int turn_tech = -1;
	for (const auto &spell_id : action) {
		auto [spell, book_idx] = player.find_spell(spell_id);
		if (spell == nullptr) {
			return false;
		}
		if (player.tech[book_idx] + (turn_tech == book_idx ? 1 : 0) < spell->get_tech()) {
			return false;
		}
		if (spell->is_tech_spell() && turn_tech != -1) {
			return false;
		}
		if (player.level() + (turn_tech != -1 ? 1 : 0) < spell->get_level()) {
			return false;
		}
		mp_left -= spell->get_cost();
		if (mp_left < 0) return false;
		if (spell->is_tech_spell()) {
			turn_tech = book_idx;
		}
	}
	return true;
}

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

void GameState::process_effect_queue(
		std::deque<EffectInstance> *effect_queue,
		std::deque<EffectInstance> *next_effect_queue) {
	while (effect_queue->size() != 0) {
		for (auto &effect_instance : *effect_queue) {
			auto &target = players[effect_instance.target_player];
			std::vector<EffectInstance> generated_effects =
				target.pipe_effect(effect_instance.target_player, effect_instance, true);
			for (const auto &e : generated_effects) {
				append_to_effect_queue(next_effect_queue, e);
			}
			effect_instance.apply(rules, target);
		}
		std::swap(effect_queue, next_effect_queue);
		next_effect_queue->clear();
	}
}

bool GameState::simulate(const std::vector<GameAction> actions) {
	if (winners().size() > 0) {
		return false;
	}
	if (actions.size() != players.size()) {
		return false;
	}
	for (size_t player_id = 0; player_id < players.size(); player_id++) {
		if (!is_valid_action(player_id, actions[player_id])) return false;
	}

	// history has its eyes on you
	history.push_back(actions);

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
		for (size_t player_id = 0; player_id < players.size(); player_id++) {
			auto &caster = players[player_id];

			// Tick down stickies that last for some number of steps.
			caster.subtract_step();

			if (spell_idx >= actions[player_id].size()) continue;

			const auto [spell, book_idx] = caster.find_spell(actions[player_id][spell_idx]);
			std::vector<EffectInstance> generated_effects =
				caster.pipe_spell(player_id, *spell);
			append_to_effect_queue(effect_queue, generated_effects);
			caster.mp -= spell->get_cost();
		}

		// Process effects triggered by spells.
		process_effect_queue(effect_queue, next_effect_queue);

		for (size_t player_id = 0; player_id < players.size(); player_id++) {
			auto &caster = players[player_id];
			if (spell_idx >= actions[player_id].size()) continue;
			const auto &spell = rules.get_spell(actions[player_id][spell_idx]);
			if (spell_idx < actions[1 - player_id].size()) {
				const auto &other_spell = rules.get_spell(actions[1 - player_id][spell_idx]);
				if (other_spell.is_counterspell() &&
						other_spell.get_counterspell_selector().selects_spell(spell)) {
					continue;
				}
			}
			for (const auto &effect : spell.get_effects()) {
				EffectInstance effect_instance(player_id, spell, effect);
				std::vector<EffectInstance> generated_effects =
					caster.pipe_effect(player_id, effect_instance, false);
				append_to_effect_queue(next_effect_queue, generated_effects);
				append_to_effect_queue(effect_queue, effect_instance);
			}
		}

		// Process effects created by spells.
		process_effect_queue(effect_queue, next_effect_queue);
	}

	for (size_t player_id = 0; player_id < players.size(); player_id++) {
		std::vector<EffectInstance> generated_effects = players[player_id].pipe_turn(player_id);
		append_to_effect_queue(effect_queue, generated_effects);
	}
	process_effect_queue(effect_queue, next_effect_queue);

	for (size_t player_id = 0; player_id < players.size(); player_id++) {
		auto &player = players[player_id];
		player.subtract_turn();
		player.mp += player.mp_regen;
		player.mp = std::min(player.mp, rules.get_mana_cap());
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
