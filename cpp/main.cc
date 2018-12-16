#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "json.h"
#include "game_rules.h"
#include "player.h"
#include "game_state.h"

using json = nlohmann::json;

const char *book_tbl_row = "  %-15s %-30s\n";
const char *spell_tbl_hdr        =         "  %-20s %-24s %-3s %-3s %-3s\n";
const char *spell_tbl_row        = "  \033[32m%-20s %-24s %-3d %-3d %-3d\033[0m\n";
const char *spell_tbl_row_locked = "  \033[31m%-20s %-24s %-3d %-3d %-3d\033[0m\n";

void cls() {
#ifdef WINDOWS
	system("cls");
#else
	system("clear");
#endif
}

void kbhit() {
#ifdef WINDOWS
	system("pause");
#else
	std::cout << "Press enter to continue." << std::endl;
	system("read");
#endif
}

int main() {
	GameRules rules("rules.json");

	cls();
	std::cout
		<<  "    ________" << std::endl
		<<  "   /        \\" << std::endl
		<<  "  /          \\" << std::endl
		<<  " /  Adrestia  \\" << std::endl
		<< " \\  ~SPELLS~  /" << std::endl
		<< "  \\          /" << std::endl
		<< "   \\________/" << std::endl
		<< std::endl;

	std::cout << "Books available" << std::endl;
	//printf(book_tbl_row, "ID", "Name");
	for (const auto &[book_id, book] : rules.get_books()) {
		printf(book_tbl_row, book_id.c_str(), book.get_name().c_str());
	}
	std::cout << std::endl;

	std::vector<std::vector<std::string>> book_choices;
	for (size_t player_id = 0; player_id < 2; player_id++) {
		std::cout << "Player " << player_id << std::endl;
		std::cout << "  How many books? > ";
		std::vector<std::string> books;
		size_t n;
		std::cin >> n;
		while (n--) {
			std::cout << "  > ";
			std::string in;
			std::cin >> in;
			books.push_back(in);
		}
		book_choices.push_back(books);
	}
	std::cout << std::endl;

	GameState state(rules, book_choices);

	while (true) {
		cls();
		std::vector<GameAction> turn;
		for (size_t player_id = 0; player_id < 2; player_id++) {
			GameAction action;
			const auto &player = state.players[player_id];
			while (true) {
				cls();
				std::cout << "It's Player " << player_id << "'s Turn" << std::endl << std::endl;
				std::cout << state << std::endl;
				for (size_t book_idx = 0; book_idx < player.books.size(); book_idx++) {
					const auto &book = *player.books[book_idx];
					const int tech = player.tech[book_idx];
					std::cout << book.get_name() << " (Level " << tech << ")" << std::endl;
					printf(spell_tbl_hdr, "ID", "Name", "Mp", "Tch", "Lvl");
					for (const auto &spell_id : book.get_spells()) {
						const auto &spell = rules.get_spell(spell_id);
						action.push_back(spell_id);
						if (state.is_valid_action(player_id, action)) {
							printf(spell_tbl_row, spell.get_id().c_str(), spell.get_name().c_str(), spell.get_cost(), spell.get_tech(), spell.get_level());
						} else {
							printf(spell_tbl_row_locked, spell.get_id().c_str(), spell.get_name().c_str(), spell.get_cost(), spell.get_tech(), spell.get_level());
						}
						action.pop_back();
					}
					std::cout << std::endl;
				}
				std::cout << "Spells to cast:";
				if (action.empty()) {
					std::cout << " [none!]";
				}
				for (const auto &spell : action) {
					std::cout << " " << spell;
				}
				std::cout << std::endl;
				int mp_left = player.mp;
				for (const auto &spell : action) {
					mp_left -= rules.get_spell(spell).get_cost();
				}
				std::cout << "Mana left: " << mp_left << std::endl;
				std::cout << "What do? (spell_id | desc spell_id | clear | pop | done)" << std::endl;
				std::cout << "> ";
				std::string in;
				std::cin >> in;
				std::cout << std::endl;
				if (in == "clear") {
					action.clear();
				} else if (in == "pop") {
					if (!action.empty()) action.pop_back();
				} else if (in == "done") {
					cls();
					kbhit();
					break;
				} else if (in.find("desc") == 0) {
					std::string spell_id;
					std::cin >> spell_id;
					try {
						std::cout << rules.get_spell(spell_id).get_text() << std::endl;
					} catch (const std::exception &e) {
						std::cout << "That spell probably doesn't exist." << std::endl;
					}
					kbhit();
				} else {
					action.push_back(in);
					if (!state.is_valid_action(player_id, action)) {
						std::cout << "Invalid action." << std::endl;
						std::cout << std::endl;
						action.pop_back();
						kbhit();
					}
				}
			}
			turn.push_back(action);
		}
		std::vector<json> events;
		state.simulate(turn, events);

		for (const json &e : events) {
			std::cout << e << std::endl;
		}

		std::cout << "The new game state is" << std::endl;
		std::cout << state;
		kbhit();

		if (state.winners().size() > 0) {
			std::cout << "---" << std::endl;
			std::cout << "Winners:";
			for (const auto &winner : state.winners()) {
				std::cout << " " << winner;
			}
			std::cout << std::endl;
			kbhit();
			break;
		}
	}
	return 0;
}
