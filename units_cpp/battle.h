/* Contains classes relating to the battle phase of Adrestia. */


#pragma once

#include <random>

#include "player_view.h"
#include "player.h"


struct Attack {
	/* Struct representing a single attack. */

	int from_player;
	int from_unit;
	int to_player;
	int to_unit;
	int damage;
};


class Battle {
	/* Struct represnting a single battle. */
	public:
		// Constructor constructs all attacks as part of the battle, but does not actually edit players' unit lists.
		Battle();
		Battle(const std::vector<Player> &players);
		static void set_seed(long seed);
		Battle(const GameRules &rules, const json &j);

		const std::vector<PlayerView> &get_players() const;
    void set_players_after(const std::vector<Player> &players);
    const std::vector<PlayerView> &get_players_after() const;
		const std::vector<Attack> &get_attacks() const;

		friend void to_json(json &j, const Battle &battle);

	private:
		std::vector<PlayerView> players;

    // Records the state of the players after the attacks have been made, but
    // before dead units have been culled.
    std::vector<PlayerView> players_after;
		std::vector<Attack> attacks;

		static std::mt19937 gen;  // The random number generator
};
