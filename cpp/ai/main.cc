#include <iostream>
#include "../json.h"
#include "../game_state.h"
#include "../game_rules.h"
#include "../game_action.h"
#include "../game_view.h"
#include "strategy.h"
#include "cfr_strategy.h"
#include "monte_strategy.h"
#include "random_strategy.h"

using json = nlohmann::json;

int main() {
	GameRules rules("rules.json");
	GameState game(rules, (std::vector<std::vector<std::string>>){{"conjuration"}, {"conjuration"}});
	std::vector<Strategy*> strategies;
	strategies.push_back(new CfrStrategy());
	strategies.push_back(new CfrStrategy());

	while (game.winners().size() == 0) {
		std::vector<GameAction> actions;
		for (size_t i = 0; i < 2; i++) {
			GameView view(game, i);
			std::cout << game.players[i].hp << " " << game.players[i].mp << " ";
			actions.push_back(strategies[i]->get_action(view));
		}
		std::cout << std::endl << json(actions) << std::endl;
		game.simulate(actions);
	}

	std::cout << json(game) << std::endl;

	for (int i = 0; i < strategies.size(); i++) {
		delete strategies[i];
	}

	std::cout << json(game.winners()) << std::endl;

	return 0;
}
