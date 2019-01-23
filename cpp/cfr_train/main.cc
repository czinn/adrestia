#include <iostream>
#include <random>
#include <chrono>
#include "../json.h"
#include "../game_state.h"
#include "../game_rules.h"
#include "../game_action.h"
#include "../game_view.h"
#include "../strategy.h"
#include "../cfr_strategy.h"

using json = nlohmann::json;

std::mt19937 gen(std::chrono::high_resolution_clock::now().time_since_epoch().count());

int main(int argc, char *argv[]) {
	GameRules rules("rules.json");
	std::vector<Strategy*> strategies;
	strategies.push_back(new CfrStrategy(rules));
	strategies.push_back(new CfrStrategy(rules));

	// Randomize book selection
	std::vector<std::vector<std::string>> book_choices(2);
	int num_books_to_select = 3;
	int num_books = 0;
	for (const auto &[book_id, book] : rules.get_books()) {
		num_books++;
		for(size_t i = 0; i < 2; i++) {
			if (book_choices[i].size() < num_books_to_select) {
				book_choices[i].push_back(book_id);
			} else {
				size_t index = gen() % num_books;
				if (index < num_books_to_select) {
					book_choices[i][index] = book_id;
				}
			}
		}
	}
	std::cout << json(book_choices) << std::endl;

	size_t num_iterations = argc > 1 ? atoi(argv[1]) : 100;

	for (size_t iterations = 0; iterations < num_iterations; iterations++) {
		GameState game(rules, book_choices);
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

		double value = game.winners().size() == 2 ? 0 : (game.winners()[0] == 0 ? 1 : -1);
		for (const auto &vec : state_vectors) {
			std::cout << "[" << json(vec) << "," << value << "]" << std::endl;
		}
	}

	for (int i = 0; i < strategies.size(); i++) {
		delete strategies[i];
	}

	return 0;
}
