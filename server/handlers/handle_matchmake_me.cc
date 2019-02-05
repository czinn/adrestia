/* Handling requests for 'matchmake_me'. */

// Us
#include "../adrestia_networking.h"

// Our related modules
#include "../adrestia_database.h"
#include "../../cpp/book.h"
#include "../../cpp/game_rules.h"

// Database
#include <pqxx/pqxx>

// System modules
#include <iostream>
using namespace std;

// JSON
#include "../../cpp/json.h"
using json = nlohmann::json;

int adrestia_networking::handle_matchmake_me(const string& log_id, const json& client_json, json& resp) {
  /* Adds this user to the waiting list for matchmaking, if there are no possible matches already waiting.
   * If there /are/ matches already waiting, the user is matched with them and a game is made.
   *
   * Accepts keys from client:
   *     HANDLER_KEY: <this function>
   *     "game_rules": The local copy the player has of the game rules.
   *     "selected_books": The books selected by the user
   *
   * Keys inserted by babysitter:
   *     "uuid": The uuid of the client
   *
   * Responds to client with keys:
   *     HANDLER_KEY: <this function>
   *     CODE_KEY: 200 if waiting, 201 if game has been made, 400 if bad books, 409 if old rules
   *     MESSAGE_KEY: "You have been put on the wait list." if waiting
   *                  "You are now in a game!" if a game made
   *                  "Incorrect number of books (got |[X]|, expected |[Y]|)" if incorrect number of books
   *                  "Invalid book: |[book_name]|" if invalid book name
   *                  "Incompatible rules" if old rules
   *     (Optional) "game_uid": The game_uid of the game that was made. If no game made, this will not be in the
   *                            response.
   *
   * Should always return 0.
   */

  cout << "[" << log_id << "] Triggered matchmake_me." << endl;
  string uuid = client_json.at("uuid");
  vector<string> selected_books = client_json.at("selected_books");
  GameRules client_rules;
  client_rules = client_json.at("game_rules");

  // Checking client rules
  pqxx::connection* psql_connection = adrestia_database::establish_psql_connection();
  GameRules game_rules = adrestia_database::retrieve_game_rules(log_id, psql_connection, 0);
  cout << "[" << log_id << "] Comparing client rules with server rules..." << endl;
  if (!(game_rules == client_rules)) {
    cout << "[" << log_id << "] Incompative rules" << endl;
    resp[adrestia_networking::HANDLER_KEY] = client_json[adrestia_networking::HANDLER_KEY];
    resp[adrestia_networking::CODE_KEY] = 409;
    resp[adrestia_networking::MESSAGE_KEY] = "Incompative rules";
    return 0;
  }

  cout << "[" << log_id << "] Matchmaking client with uuid |" << uuid << "|..." << endl;

  cout << "[" << log_id << "] checking number of books..." << endl;
  if (selected_books.size() == 0 || selected_books.size() > 3) {
    cout << "[" << log_id << "] Incorrect number of selected books: |" << selected_books.size() << "|" << endl;
    resp[adrestia_networking::HANDLER_KEY] = client_json[adrestia_networking::HANDLER_KEY];
    resp[adrestia_networking::CODE_KEY] = 400;
    resp[adrestia_networking::MESSAGE_KEY] = "Incorrect number of books (got |" + to_string(selected_books.size()) + "|, expected |3|)";
    return 0;
  }
  cout << "[" << log_id << "] got good number of books." << endl;

  // Checking books
  cout << "[" << log_id << "] checking book names..." << endl;
  const std::map<string, Book>& book_map = game_rules.get_books();

  bool incorrect_book = false;
  string bad_book;
  for (unsigned int i = 0; i < selected_books.size(); i += 1) {
    string selected_book = selected_books[i];
    if (book_map.find(selected_book) == book_map.end()) {
      incorrect_book = true;
      bad_book = selected_book;
      break;
    }
  }

  if (incorrect_book) {
    cout << "[" << log_id << "] got invalid requested book |" << bad_book << "|." << endl;
    resp[adrestia_networking::HANDLER_KEY] = client_json[adrestia_networking::HANDLER_KEY];
    resp[adrestia_networking::CODE_KEY] = 400;
    resp[adrestia_networking::MESSAGE_KEY] = "Invalid book: |" + bad_book + "|";
    return 0;
  }
  cout << "[" << log_id << "] Selected books all seem okay." << endl;
  cout << "[" << log_id << "] Matchmaking uuid |" << uuid << "| in database." << endl;

  json database_json = adrestia_database::matchmake_in_database(log_id, psql_connection, uuid, selected_books);
  delete psql_connection;

  string game_uid = database_json["game_uid"];
  if (game_uid.compare("") != 0) {
    cout << "[" << log_id << "] A new game has been made (game_id |" << database_json["game_uid"] << "|)!" << endl;
    resp["game_uid"] = database_json.at("game_uid");
    resp[adrestia_networking::HANDLER_KEY] = client_json[adrestia_networking::HANDLER_KEY];
    resp[adrestia_networking::CODE_KEY] = 201;
    resp[adrestia_networking::MESSAGE_KEY] = "You are now in a game!";
    return 0;
  } else {
    // No new game was made
    cout << "[" << log_id << "] UUID |" << uuid << "| is now on the waiting list." << endl;
    resp[adrestia_networking::HANDLER_KEY] = client_json[adrestia_networking::HANDLER_KEY];
    resp[adrestia_networking::CODE_KEY] = 200;
    resp[adrestia_networking::MESSAGE_KEY] = "You have been put on the wait list.";
    return 0;
  }

  return 1;
}
