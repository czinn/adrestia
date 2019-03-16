/* Handling requests for 'matchmake_me'. */

// Us
#include "../adrestia_networking.h"

// Our related modules
#include "../adrestia_database.h"
#include "../adrestia_hexy.h"
#include "../../cpp/book.h"
#include "../../cpp/game_rules.h"

// Database
#include <pqxx/pqxx>

// System modules
#include <iostream>
using namespace std;
using namespace adrestia_networking;

// JSON
#include "../../cpp/json.h"
using json = nlohmann::json;

int adrestia_networking::handle_matchmake_me(const Logger& logger, const json& client_json, json& resp) {
  /* Called to express desire to enter a multiplayer game.
   * If [target_friend_code] is specified, will only match with that specified player.
   * Otherwise, either enters the public queue or gets matched with someone from it.
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
   */

  string uuid = client_json.at("uuid");
  vector<string> selected_books = client_json.at("selected_books");
  GameRules client_rules = client_json.at("game_rules");

  // Checking client rules
  adrestia_database::Db db;
  GameRules game_rules = db.retrieve_game_rules(0);
  logger.trace("Comparing client rules with server rules...");
  if (!(game_rules == client_rules)) {
    logger.trace("Incompative rules");
    resp[HANDLER_KEY] = client_json[HANDLER_KEY];
    resp[CODE_KEY] = 400;
    resp[MESSAGE_KEY] = "Incompative rules";
    return 0;
  }

  logger.trace("Matchmaking client with uuid |%s|", uuid.c_str());
  logger.trace("Checking number of books...");
  if (selected_books.size() == 0 || selected_books.size() > 3) {
    logger.trace("Incorrect number of selected books: |%zu|", selected_books.size());
    resp[HANDLER_KEY] = client_json[HANDLER_KEY];
    resp[CODE_KEY] = 400;
    resp[MESSAGE_KEY] = "Bad number of books";
    return 0;
  }
  logger.trace("Got good number of books.");

  // Check books
  logger.trace("Checking book names...");
  const map<string, Book>& book_map = game_rules.get_books();

  string bad_book = "";
  for (unsigned int i = 0; i < selected_books.size(); i += 1) {
    string selected_book = selected_books[i];
    if (book_map.find(selected_book) == book_map.end()) {
      bad_book = selected_book;
      break;
    }
  }

  if (bad_book != "") {
    logger.info("Got invalid requested book |%s|", bad_book.c_str());
    resp[HANDLER_KEY] = client_json[HANDLER_KEY];
    resp[CODE_KEY] = 400;
    resp[MESSAGE_KEY] = "Bad book: " + bad_book;
    return 0;
  }

  logger.trace("Selected books all seem okay. Matchmaking in database.");

  std::string target_friend_code = client_json.value("target_friend_code", "");
  std::string target_uuid = "";
  if (target_friend_code != "") {
    auto result = db.query(R"sql(
      SELECT uuid FROM adrestia_accounts WHERE friend_code = ?
    )sql")(target_friend_code)();
    if (result.size() > 0) {
      target_uuid = result[0][0].as<string>();
    } else {
      logger.warn_() << "No uuid for friend code " << target_friend_code << endl;
      resp[HANDLER_KEY] = client_json[HANDLER_KEY];
      resp[CODE_KEY] = 400;
      resp[MESSAGE_KEY] = "Bad friend code";
    }
  }

  auto query_result =
    (target_uuid == "")
    ? db.query(R"sql(
      SELECT uuid, selected_books
      FROM adrestia_match_waiters
      WHERE target_uuid = ''
      LIMIT 1
      FOR UPDATE
    )sql")()
    : db.query(R"sql(
      SELECT uuid, selected_books
      FROM adrestia_match_waiters
      WHERE uuid = ? AND target_uuid = ?
      LIMIT 1
      FOR UPDATE
    )sql")(target_uuid)(uuid)();
    // intentionally swapped order of uuid and target_uuid; we're looking for
    // our-target-who-has-also-targeted-us

  if (query_result.size() > 0) {
    string waiting_uuid = query_result[0]["uuid"].as<string>();

    // Create unique game_uid
    string game_uid = "";
    for (size_t i = 0; i < 1000; i++) {
      game_uid = adrestia_hexy::hex_urandom(adrestia_database::GAME_UID_LENGTH);
      if (db.query("SELECT 1 FROM adrestia_games WHERE game_uid = ?")(game_uid)().size() == 0) {
        break;
      }
    }
    if (game_uid == "") throw string("Failed to create unique game ID.");

    GameRules rules = db.retrieve_game_rules(0);
    vector<string> waiting_selected_books =
      adrestia_database::sql_array_to_vector(query_result[0]["selected_books"].as<string>());

    size_t creator_player_id = 0; // TODO: charles: Randomize this, though it shouldn't matter
    vector<vector<string>> books = { selected_books, waiting_selected_books };
    if (creator_player_id == 1) {
      swap(books[0], books[1]);
    }
    GameState game_state(rules, books);

    db.query(R"sql(
      INSERT INTO adrestia_games (game_uid, creator_uuid, activity_state, game_state, game_rules_id)
      SELECT ?, ?, 0, ?, id
      FROM adrestia_rules ORDER BY -id LIMIT 1
    )sql")(game_uid)(uuid)(json(game_state).dump())();

    db.query(R"sql(
      INSERT INTO adrestia_players (game_uid, user_uid, player_id, player_state, last_move_time)
      VALUES (?, ?, ?, 0, NOW())
    )sql")(game_uid)(uuid)(creator_player_id)();

    db.query(R"sql(
      INSERT INTO adrestia_players (game_uid, user_uid, player_id, player_state, last_move_time)
      VALUES (?, ?, ?, 0, NOW())
    )sql")(game_uid)(waiting_uuid)(1 - creator_player_id)();

    logger.debug("Removing waiting uuid |%s|", waiting_uuid.c_str());
    db.query("DELETE FROM adrestia_match_waiters WHERE uuid = ?")(waiting_uuid)();
    db.commit();

    logger.info_() << "A new game has been made (game_id |" << game_uid << "|)!" << endl;
    resp["game_uid"] = game_uid;
    resp[HANDLER_KEY] = client_json[HANDLER_KEY];
    resp[CODE_KEY] = 201;
    resp[MESSAGE_KEY] = "You are now in a game!";
    return 0;
  }

  logger.info("No possible matches are waiting. We shall become a waiter.");

  db.query(R"sql(
    INSERT INTO adrestia_match_waiters (uuid, selected_books, target_uuid)
    VALUES (?, ?, ?)
    ON CONFLICT (uuid) DO UPDATE SET selected_books = ?
  )sql")(uuid)(selected_books)(target_uuid)
    (selected_books)();

  logger.trace("Committing transaction.");
  db.commit();

  logger.info_() << "UUID |" << uuid << "| is now on the waiting list." << endl;
  resp[HANDLER_KEY] = client_json[HANDLER_KEY];
  resp[CODE_KEY] = 200;
  resp[MESSAGE_KEY] = "You have been put on the wait list.";
  return 0;
}
