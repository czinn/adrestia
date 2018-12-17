#include <iostream>
#include "../json.h"
#include "../game_state.h"
#include "../game_rules.h"
#include "../game_action.h"
#include "../game_view.h"
#include "../strategy.h"
#include "../cfr_strategy.h"

using json = nlohmann::json;

int main() {
	GameRules rules("cfr_train/rules.json");
	GameState game(rules, (std::vector<std::vector<std::string>>){{"conjuration"}, {"conjuration"}});
	std::vector<Strategy*> strategies;
	strategies.push_back(new CfrStrategy());
	strategies.push_back(new CfrStrategy());

	std::vector<std::vector<double>> state_vectors;
	while (game.winners().size() == 0) {
		std::vector<GameAction> actions;
		for (size_t i = 0; i < 2; i++) {
			GameView view(game, i);
			actions.push_back(strategies[i]->get_action(view));
		}
		game.simulate(actions);
		state_vectors.push_back(cfr_state_vector(game));
	}

	for (int i = 0; i < strategies.size(); i++) {
		delete strategies[i];
	}

	double value = game.winners().size() == 2 ? 0 : (game.winners()[0] == 0 ? 1 : -1);
	for (const auto &vec : state_vectors) {
		std::cout << "[" << json(vec) << "," << value << "]" << std::endl;
	}

	return 0;
}
