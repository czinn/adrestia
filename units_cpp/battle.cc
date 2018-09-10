#include "battle.h"
#include "player_view.h"
#include "player.h"


std::mt19937 Battle::gen;  // The random number generator


//------------------------------------------------------------------------------
// CONSTRUCTORS
//------------------------------------------------------------------------------
Battle::Battle() {}


Battle::Battle(const std::vector<Player> &players) {
	/*! \brief Constructs the Battle object, including all attacks. Does not actually edit players' units lists. */

	// Record the initial PlayerViews from each player
	for (auto &&player : players) {
		this->players.push_back(PlayerView(player));
	}

	// For each player...
	for (size_t i = 0; i < players.size(); i++) {
		const Player &player = players[i];  // The currently-scanned player
		std::vector<std::pair<int, int>> targets;  // A list of possible target "tiles": <player_id, unit_unique_id>

		// For each /other/ player...
		for (size_t j = 0; j < players.size(); j++) {
			if (j == i) {
				// Skip over ourselves
				continue;
			}

			// For each of the other player's units...
			for (auto &&[unit_id, unit] : players[j].units) {
				// For each "tile" of the other player's width...
				for (int k = 0; k < unit.kind.get_width(); k++) {
					targets.push_back(std::make_pair(j, unit_id));  // Add this tile to targets
				}
			}
		}

		// For each of the player's units
		for (auto &&[unit_id, unit] : player.units) {
			if (unit.build_time > 0) {
				// If the unit is still being built, it does not participate in this battle and is ignored.
				continue;
			}

			// For each of this unit's attacks, assign a target from targets and create an appropriate Attack instance
			// Store the attack instance in attacks
			const auto &unit_attacks = unit.kind.get_attack();
			for (int dmg : unit_attacks) {
				int target_index = Battle::gen() % targets.size();
				attacks.push_back(Attack{
										 (int)i, unit_id,
										 targets[target_index].first,
										 targets[target_index].second,
										 dmg
										}
								 );
			}
		}
	}
}

Battle::Battle(const GameRules &rules, const json &j) {
	/* Construct a battle from JSON; the GameRules reference is necessary to use the units within. */
	for (auto &p : j["players"]) {
		players.push_back(PlayerView(rules, p));
	}
	for (auto &a : j["attacks"]) {
		attacks.push_back(Attack{ a[0], a[1], a[2], a[3], a[4] });
	}
}

//------------------------------------------------------------------------------
// GETTERS AND SETTERS
//------------------------------------------------------------------------------
void Battle::set_seed(long seed) {
	/*! \brief sets the seed for ALL Battles */
	Battle::gen.seed(seed);
}


const std::vector<PlayerView> &Battle::get_players() const {
	/*! \brief Return the PlayerViews of all the players in this battle. */
	return players;
}

void Battle::set_players_after(const std::vector<Player> &players) {
  players_after.clear();
	for (auto &&player : players) {
		players_after.push_back(PlayerView(player));
	}
}

const std::vector<PlayerView> &Battle::get_players_after() const {
  return players_after;
}


const std::vector<Attack> &Battle::get_attacks() const {
	/*! \brief Returns all the Attacks of this battle. */
	return attacks;
}


//------------------------------------------------------------------------------
// OTHER METHODS
//------------------------------------------------------------------------------
void to_json(json &j, const Battle &battle) {
	// Copy the players involved in the battle
	for (const auto &player : battle.players) {
		j["players"].push_back(player);
	}

	// Copy the attacks involved in the battle
	for (const auto &it : battle.attacks) {
		j["attacks"].push_back({ it.from_player, it.from_unit, it.to_player, it.to_unit, it.damage });
	}
}
