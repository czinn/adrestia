#include "game_state.h"

#include <cassert>
#include <vector>

#include "action.h"
#include "battle.h"
#include "player.h"
#include "player_view.h"


//------------------------------------------------------------------------------
// CONSTRUCTORS
//------------------------------------------------------------------------------
GameState::GameState(const GameRules &rules, int num_players):
		rules(rules),
		turn(0),
		players_ready(0)
{
	for (int i = 0; i < num_players; i++) {
		players.push_back(Player(rules));
	}
	begin_turn();
}


GameState::GameState(const GameRules &rules, const json &j):
		rules(rules)
{
	for (auto &player : j["players"]) {
		players.push_back(Player(rules, player));
	}
	for (auto &turn_actions : j["action_log"]) {
		action_log.push_back(std::vector<std::vector<Action>>(players.size()));
		int i = action_log.size() - 1;
		int j = 0;
		for (auto &player_actions : turn_actions) {
			for (auto &action : player_actions) {
				action_log[i][j].push_back(action);
			}
			j++;
		}
	}
	for (auto &battle : j["battles"]) {
		battles.push_back(std::shared_ptr<Battle>(new Battle(rules, battle)));
	}
	turn = j["turn"];
	players_ready = j["players_ready"];
}


GameState::GameState(const GameState &game_state):
		rules(game_state.rules),
		players(game_state.players),
		action_log(game_state.action_log),
		battles(game_state.battles),
		turn(game_state.turn),
		players_ready(game_state.players_ready)
{}


GameState::GameState(const GameView &view, const std::vector<Tech> &techs):
		rules(*view.rules),
		action_log(view.action_log),
		battles(*view.battles),
		turn(view.turn),
		players_ready(0)
{
	/* Creates a GameState from the given view, as well as all hidden information
	 *     (which allows reconstruction of the things missing from the view)
	 */
	int i = 0;
	for (auto &player : view.players) {  // For each of the players in the view
		if (i == view.view_index) {
			players.push_back(view.view_player);
		}
		else {
			players.push_back(Player());
			players[i].units = std::map<int, Unit>(player.units);
			players[i].alive = player.alive;
			players[i].coins = player.coins;
			players[i].next_unit = player.units.rbegin()->first + 1;
		}

		players[i].tech = techs[i];
		i++;
	}
}


//------------------------------------------------------------------------------
// GETTERS AND SETTERS
//------------------------------------------------------------------------------
void GameState::get_view(GameView &view, int player) const {
	/* Returns the GameView form of this GameState, geared for the requested
	 * player. */

	view.rules = &rules;
	// TODO: charles: What the hell C++?? Why do I have to do this?
	Player p(players[player]);
	view.view_index = player;
	view.view_player = p;
	view.players.clear();
	for (const auto &player : players) {
		view.players.push_back(PlayerView(player));
	}

	// We start with the full action log...
	view.action_log = action_log;

	// Remove all actions not visible to the requested player
	for (int scanned_turn = 0; scanned_turn < action_log.size(); scanned_turn++) {
		for (int scanned_player = 0; scanned_player < players.size(); scanned_player++) {
			// A players' own actions are always visible to themselves
			if (scanned_player == player) {
				continue;
			}

			// If the currently-scanned turn is the last one
			if (scanned_turn == action_log.size() - 1) {
				// This last turn contains an enemy's current move orders, and should
				// be removed entirely.
				view.action_log[scanned_turn][scanned_player].clear();
			}
			else {
				// Remove all CHOOSE_TECH actions from past turns.
				auto &actions = view.action_log[scanned_player][scanned_turn];
				actions.erase(std::remove_if(actions.begin(),
				                             actions.end(),
				                             [](const Action &a) { return a.get_type() == CHOOSE_TECH; }
				                            ),
				              actions.end()
				             );
			}
		}
	}

	view.battles = &battles;  // All battles are globally visible
	view.turn = turn;
}


std::vector<int> GameState::get_winners() const {
	/* Returns the player number of all winners. More specifically:
	 * - Returns the player number of any single winner.
	 * - Returns all players' numbers in case of a draw.
	 * - Returns no players' numbers in case of the game still being ongoing.
	 */

	std::vector<int> winners;

	// Find all players with their general still alive and add them to winners
	for (size_t i = 0; i < players.size(); i++) {
		for (auto it = players[i].units.begin(); it != players[i].units.end(); it++) {
			if (it->second.kind.get_id() == "general") {
				winners.push_back(i);
			}
		}
	}


	if (winners.size() == 1) {
		// Only a single player has their general still alive - they are the winner.
		return winners;
	}
	else if (winners.size() == 0) {
		// No generals remain - the game is a draw. Return all players' numbers.
		for (size_t i = 0; i < players.size(); i++) {
			winners.push_back(i);
		}
		return winners;
	} else {
		// More than one general still remains - the game is still ongoing. Return
		// an empty vector.
		return std::vector<int>();
	}
}


const GameRules &GameState::get_rules() const { return rules; }
const std::vector<Player> &GameState::get_players() const { return players; }
const std::vector<std::vector<std::vector<Action>>> &GameState::get_action_log() const {
	return action_log;
}
const std::vector<std::shared_ptr<Battle>> &GameState::get_battles() const {
	return battles;
}
int GameState::get_turn() const { return turn; }


//------------------------------------------------------------------------------
// OTHER METHODS
//------------------------------------------------------------------------------
void to_json(json &j, const GameState &game_state) {
	j["players"] = game_state.players;
	j["action_log"] = game_state.action_log;
	for (const auto battle : game_state.battles) {
		j["battles"].push_back(*battle);
	}
	j["turn"] = game_state.turn;
	j["players_ready"] = game_state.players_ready;
}


void GameState::begin_turn() {
	/* Advance the turn counter, and request all players begin their turns. */

	turn++;
	players_ready = 0;

	// Request all players begin their turn
	for (auto &player : players) {
		player.begin_turn();
	}

	// Prepare a new vector of actions for each player, for the current turn.
	action_log.push_back(std::vector<std::vector<Action>>(players.size()));
}


void GameState::execute_battle() {
	/* Creates a new Battle object and adds it to the record. */

	Battle *b = new Battle(players);
	battles.push_back(std::shared_ptr<Battle>(b));

	// Do all the damage from the attacks
	for (auto &atk : b->get_attacks()) {
		Unit &unit = players[atk.to_player].units.at(atk.to_unit);
		unit.shields -= atk.damage;
		if (unit.shields < 0) {
			unit.health += unit.shields;
			unit.shields = 0;
		}
	}
	b->set_players_after(players);  // Update players_after

	// Kill dead units, restore shields for alive units
	for (auto &player : players) {
		for (auto it = player.units.begin(); it != player.units.end();) {
			if (it->second.health <= 0) {
				auto to_delete = it;
				it++;
				player.units.erase(to_delete);
			}
			else {
				it->second.shields = it->second.kind.get_shields();
				it++;
			}
		}
	}
	if (get_winners().size() == 0) {
		begin_turn();
	}
}


bool GameState::perform_action(int player, const Action &action) {
	// Game is already over - all actions illegal
	if (get_winners().size() > 0) return false;
	// Nonexistent player
	if ((size_t)player >= players.size()) return false;
	Player &p = players[player];
	// Dead player
	if (!p.alive) return false;

	int total_tech = p.tech.red + p.tech.green + p.tech.blue;
	if (action.get_type() == CHOOSE_TECH) {
		// The player should have no more tech increases this turn
		if (total_tech >= turn) return false;
		Colour colour = action.get_colour();
		if (colour == BLACK) return false;

		// At this point, the action is valid
		action_log[turn - 1][player].push_back(action);
		p.tech.increment(colour);
		return true;
	} else if (action.get_type() == BUILD_UNITS) {
		// Haven't selected resources yet this turn
		if (total_tech < turn) return false;
		// Already built units this turn
		if (action_log[turn - 1][player].size() >= 2) return false;
		// Past unit cap
		if (action.get_units().size() + p.units.size() > (size_t)rules.get_unit_cap()) {
			return false;
		}

		int total_cost = 0;
		std::vector<const UnitKind*> build_order;  // The UnitKinds the player wants to build
		for (auto &name : action.get_units()) {
			const UnitKind &kind = rules.get_unit_kind(name);
			// Unit is unbuyable
			if (kind.get_tech() == nullptr) return false;
			// Player does not have the correct tech
			if (!p.tech.includes(*kind.get_tech())) return false;
			total_cost += kind.get_cost();
			build_order.push_back(&kind);
		}

		if (p.coins < total_cost) return false;

		// Action is valid
		action_log[turn - 1][player].push_back(action);
		for (auto &kind : build_order) {
			p.build_unit(*kind);
		}
		p.coins -= total_cost;
		players_ready++;

		// If this is the last player we were waiting for
		if (players_ready == (int)players.size()) {
			execute_battle();
		}
		return true;
	}
	return false;
}
