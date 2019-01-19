/* Handling requests for 'matchmake_me'. */

// Us
#include "../adrestia_networking.h"

// Our related modules
#include "../adrestia_database.h"

// Database
#include <pqxx/pqxx>

// System modules
#include <iostream>
using namespace std;

// JSON
#include "../../units_cpp/json.h"
using json = nlohmann::json;


int adrestia_networking::handle_matchmake_me(const string& log_id, const json& client_json, json& resp) {
  /* Adds this user to the waiting list for matchmaking, if there are no possible matches already waiting.
   * If there /are/ matches already waiting, the user is matched with them and a game is made.
   *
   * Accepts keys from client:
   *     HANDLER_KEY: <this function>
   *     "selected_books": The books selected by the user
   *
   * Keys inserted by babysitter:
   *     "uuid": The uuid of the client
   *
   * Responds to client with keys:
   *     HANDLER_KEY: <this function>
   *     CODE_KEY: 200 if waiting, 201 if game has been made
   *     MESSAGE_KEY: "You have been put on the wait list." if waiting; "You are now in a game!" if a game made
   *     (Optional) "game_uid": The game_uid of the game that was made. If no game made, this will not be in the
   *                            response.
   *
   * Should always return 0.
   */

  cout << "[" << log_id << "] Triggered matchmake_me." << endl;
  string uuid = client_json.at("uuid");
  vector<string> selected_books = client_json.at("selected_books");

  cout << "[" << log_id << "] Matchmaking client with uuid |" << uuid << "|..." << endl;

  pqxx::connection* psql_connection = adrestia_database::establish_psql_connection();
  json database_json = adrestia_database::matchmake_in_database(log_id, psql_connection, uuid, selected_books);

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
