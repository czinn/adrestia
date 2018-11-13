#include "game_state.h"

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

bool GameState::operator==(const GameState &other) const {
	return this == &other;
}

//------------------------------------------------------------------------------
// GETTERS
//------------------------------------------------------------------------------
const std::vector<Player> &GameState::get_players() const { return players; }

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

	size_t max_action_length = 0;
	for (const auto &action : actions) {
		max_action_length = std::max(max_action_length, action.size());
	}

	// Cast spells.
	for (size_t spell_idx = 0; spell_idx < max_action_length; spell_idx++) {
		for (size_t player_id = 0; player_id < players.size(); player_id++) {
			auto &caster = players[player_id];
			auto &natural_target = players[(player_id + 1) % players.size()];
			if (spell_idx >= actions[player_id].size()) continue;
			const auto [spell, book_idx] = caster.find_spell(actions[player_id][spell_idx]);
			// TODO: Counterspells.
			caster.mp -= spell->get_cost();
			for (const auto &effect : spell->get_effects()) {
				// TODO: Pipe effect through stickies.
				const auto amount = effect.get_amount();
				auto &target = effect.get_targets_self() ? caster : natural_target;
				switch (effect.get_kind()) {
					case EK_TECH:
						// Ignores targets_self.
						caster.tech[book_idx] += amount;
						break;
					case EK_HEALTH:
						target.hp += amount;
						break;
					case EK_MANA:
						target.mp += amount;
						break;
					case EK_REGEN:
						target.mp_regen += amount;
						break;
					case EK_STICKY:
						// TODO: Stickies should exist.
						break;
				}
			}
		}
	}

	// TODO: Stickies should take effect.
	for (size_t player_id = 0; player_id < players.size(); player_id++) {
		auto &player = players[player_id];
		player.mp += player.mp_regen;
		player.mp = std::min(player.mp, rules.get_mana_cap());
	}

	// TODO: Effects produced by god-stickies should fire.

	// history has its eyes on you
	history.push_back(actions);
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
