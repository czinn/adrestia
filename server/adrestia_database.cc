/* The database interface component for adrestia's networking. */

// Us
#include "adrestia_database.h"

// Our related modules
#include "adrestia_hexy.h"
#include "logger.h"
#include "../cpp/game_rules.h"
#include "../cpp/game_state.h"

// Database modules
#include <pqxx/pqxx>

// System modules
#include <cstdlib>  // getenv
#include <iostream>
#include <string>
#include <string.h>
using namespace std;

// JSON
#include "../cpp/json.h"
using json = nlohmann::json;


// Performs a query using [work] with printf-style substitutions.
// You must escape values yourself when using this.
pqxx::result run_query(const Logger& logger, pqxx::work& work, const char* query_format, ...) {
  va_list args;

  // First, use [vsnprintf] to figure out how long the query needs to be.
  va_start(args, query_format);
  size_t needed = vsnprintf(nullptr, 0, query_format, args) + 1;
  va_end(args);

  // Then, use [vsprintf] to format it and run it.
  va_start(args, query_format);
  char *query = new char[needed];
  vsprintf(query, query_format, args);
  logger.trace_() << "Running SQL (old way):" << query << endl;

  pqxx::result result;
  result = work.exec(query);

  delete[] query;
  va_end(args);
  return result;
}


string adrestia_database::hash_password(const string& password, const string& salt) {
  // Get expected hashed password.
  // The hashing of the password only in the case that the account exists is bad for security, but good for speed!
  string salt_and_password = salt + password;
  unsigned char* hash_of_salt_and_password;
  unsigned int hash_of_salt_and_password_length;
  adrestia_hexy::digest_message(salt_and_password.c_str(),
                                salt_and_password.length(),
                                &hash_of_salt_and_password,
                                &hash_of_salt_and_password_length
                               );
  string good_string = string(reinterpret_cast<const char *>(hash_of_salt_and_password),
                                   (size_t)hash_of_salt_and_password_length);
  // Matches the OPENSSL_malloc in adrestia_hexy::digest_message.
  OPENSSL_free(hash_of_salt_and_password);

  return good_string;
}


vector<string> sql_array_to_vector(const string& sql_array) {
  /* @brief Converts a 1-D sql array string like '{"fleep", "floop"}' into a vector of strings. */

  if (sql_array.length() < 2) {
    return vector<string>();
  }

  // Remove capping {}
  std::string trimmed = sql_array.substr(1, sql_array.length() - 2);
  char* sql_interior_string = strdup(trimmed.c_str());

  vector<string> result;
  const char* pch = strtok(sql_interior_string, ",");
  while (pch != NULL) {
    result.push_back(string(pch));
    pch = strtok(NULL, ",");
  }
  free(sql_interior_string);
  return result;
}


template<typename T>
string vector_to_sql_array(pqxx::work& work, const vector<T>& vec) {
  string result = "ARRAY[";
  for (size_t i = 0; i < vec.size(); i += 1) {
    if (i > 0) {
      // First element has no comma before it.
      result += ',';
    }
    result += work.quote(vec[i]);
  }
  result += ']';
  return result;
}

GameRules adrestia_database::retrieve_game_rules(
  const Logger& logger,
  pqxx::connection& conn,
  int id
) {
  /* @brief Gets the game rules with the given id
   *
   * @param logger: Logger
   * @param conn: The pqxx PostgreSQL connection.
   * @param id: Id of the game rules, from the game table. An id of 0 gets the
   *            most recent set of rules.
   *
   * @returns: A GameRules object constructed from the database record
   */

  logger.trace(
      "retrieve_game_rules called with args: \n"
      "    id: |%d|", id);

  pqxx::work work(conn);

  if (id == 0) {
    auto search_result = run_query(logger, work, R"sql(
      SELECT game_rules FROM adrestia_rules ORDER BY -id LIMIT 1
      )sql");
    if (search_result.size() == 0) {
      throw string("No game rules records in database");
    }
    return json::parse(search_result[0][0].as<string>());
  } else {
    auto search_result = run_query(logger, work, R"sql(
      SELECT game_rules FROM adrestia_rules WHERE id = %d
      )sql", id);
    if (search_result.size() == 0) {
      throw string("Could not find game rules");
    }
    return json::parse(search_result[0][0].as<string>());
  }
}


void adrestia_database::conclude_game_in_database(
  const Logger& logger,
  pqxx::connection& psql_connection,
  const std::string& game_uid,
  const std::string& uuid,
  int game_result
) {
  /* @brief Concludes game in the database. How the game concluded is a parameter.
   *
   * @param logger: Logger
   * @param psql_connection: The PostgreSQL connection.
   * @param game_uid: The game_uid we will be concluding.
   * @param uuid: The player/babysitter concluding the game.
   * @param game_result: How the game concluded. Possible options are:
   *        -2: The game ended in a tie
   *        -1: This player aborted the game
   *        0: This player lost the game
   *        1: This player won the game
   *
   * @exception string: Throws a string if could not find a uuid/game_uid pairing in adrestia_players when aborting
   *
   * @returns Nothing
   */

  logger.trace(
    "conclude_game_in_database called with args:\n"
    "    game_uid: |%s|"
    "    uuid: |%s|",
    game_uid.c_str(), uuid.c_str()
  );

  pqxx::work work(psql_connection);

  switch(game_result) {
    case -2: {
      // This is a tie. Ties are special since we don't need to find out who the winner was.
      logger.debug("Tying game |%s|...",
                   game_uid.c_str()
                  );
      run_query(logger, work,
                R"sql(
                  UPDATE adrestia_games
                    SET activity_state = 1
                    WHERE game_uid = %s
                )sql",
                work.quote(game_uid).c_str()
               );

      // Nothing more needs to be done for a tie.
      logger.trace("conclude_game_in_database concluded.");
      return;
    }
    case -1: {
      // This is an abort. Aborts are special since we must mark the player as aborted, too.
      logger.debug(
        "Aborting player uuid |%s| in game |%s|...",
        uuid.c_str(), game_uid.c_str()
      );
      run_query(logger, work,
                R"sql(
                  UPDATE adrestia_players
                    SET player_state = -1
                    WHERE game_uid = %s
                      AND user_uid = %s
                )sql",
                work.quote(game_uid).c_str(),
                work.quote(uuid).c_str()
               );
      break;
    }
  }

  logger.trace("Finding player |%s|'s id in game |%s|...",
               uuid.c_str(), game_uid.c_str()
              );

  auto search_result = run_query(logger, work,
                                 R"sql(
                                   SELECT player_id
                                     FROM adrestia_players
                                     WHERE game_uid = %s
                                       AND user_uid = %s
                                 )sql",
                                 work.quote(game_uid).c_str(),
                                 work.quote(uuid).c_str()
                                );
  if (search_result.size() == 0) {
    string error_string = "Could not find player |" + uuid + "| as player of game |" + game_uid + "|!";
    logger.error(error_string.c_str());
    throw string(error_string);
  }

  int player_id = search_result[0][0].as<int>();
  logger.trace("Player |%s| was id |%d| in game |%s|.",
               uuid.c_str(), player_id, game_uid.c_str()
              );

  int winner_id = player_id;
  int game_final_state = 1;

  switch(game_result) {
    case -1: {
      // This is an abort. The player lost the game, and final state is -1.
      game_final_state = -1;
    }
    case 0: {
      // This is a loss. The player lost the game.
      if (player_id == 0) {
        winner_id = 1;
      }
      else if (player_id == 1) {
        winner_id = 0;
      }
      break;
    }
  }

  logger.trace("Updating game |%s| to have final state |%s|, winner |%s|.",
               game_uid.c_str(), to_string(game_final_state).c_str(), to_string(winner_id).c_str()
              );
  run_query(logger, work,
            R"sql(
              UPDATE adrestia_games
                SET activity_state = %s,
                    winner_id = %s
                WHERE game_uid = %s
            )sql",
            work.quote(to_string(game_final_state)).c_str(),
            work.quote(winner_id).c_str(),
            work.quote(game_uid).c_str()
           );

  logger.trace("Committing transaction...");
  work.commit();

  logger.trace("conclude_game_in_database concluded.");
}


json adrestia_database::retrieve_player_info_from_database (
  const Logger& logger,
  pqxx::connection& psql_connection,
  const string& game_uid,
  const string& uuid
) {
  /* @brief Retrieves information about a player in a specific game.
   *
   * @param logger: Logger
   * @param psql_connection: The pqxx PostgreSQL connection.
   * @param game_uid: The game_uid we will be getting information from.
   * @param uuid: The user whose information we will be getting.
   *
   * @exception string: If no game player can be found matching the
   *                    specific game_uid/uuid pair, a string will
   *                    be thrown.
   *
   * @returns json containing the following keys:
   *          "player_id": The player id in the game, as integer
   *          "player_state": The player state in the game, as integer
   *          "player_move": The player move, if any, as an array. Null if null.
   */

  logger.trace(
    "retrieve_player_info_from_database called with args:\n"
    "    game_uid: |%s|"
    "    uuid: |%s|",
    game_uid.c_str(), uuid.c_str()
  );

  pqxx::work work(psql_connection);

  auto search_result = run_query(logger, work,
                                 R"sql(
                                   SELECT player_id, player_state, player_move
                                     FROM adrestia_players
                                     WHERE game_uid = %s
                                       AND user_uid = %s
                                 )sql",
                                 work.quote(game_uid).c_str(),
                                 work.quote(uuid).c_str()
                                );

  if (search_result.size() == 0) {
    string error_string = "Could not find player for uuid |" + uuid + "|, game_uid |" + game_uid + "|.";
    logger.error(error_string.c_str());
    throw string(error_string);
  }

  logger.trace("Successfully found player.");

  json return_var;
  return_var["player_id"] = search_result[0][0].as<int>();
  return_var["player_state"] = search_result[0][1].as<int>();
  if (search_result[0][2].is_null()) {
    return_var["player_move"] = nullptr;
  }
  else {
    return_var["player_move"] = json::parse(search_result[0][2].as<string>());
  }

  logger.trace("Committing transaction...");
  work.commit();

  logger.trace("retrieve_player_info_from_database concluded.");

  return return_var;
}



json adrestia_database::retrieve_gamestate_from_database (
  const Logger& logger,
  pqxx::connection& psql_connection,
  const string& game_uid,
  GameRules &game_rules,
  vector<json> &last_events
) {
  /* @brief Retrieves the gamestate of a target game.
   *
   * @param logger: Logger
   * @param psql_connection: The pqxx PostgreSQL connection.
   * @param game_uid: The game_uid whose game_state will be returned.
   *                  If there is no such game, or the game has no state,
   *                      this will be an empty string.
   * @param game_rules: A GameRules object that should be populated with the
   *                    game's rules.
   * @param last_events: A vector that should be populated with the most recent
   *                     events from simulation.
   * @returns: json representation of the game state
   */

  logger.trace(
    "retrieve_gamestate_from_database called with args:\n"
    "    game_uid: |%s|",
    game_uid.c_str()
  );

  pqxx::work work(psql_connection);

  auto search_result = run_query(logger, work, R"sql(
    SELECT game_rules, game_state, last_events
    FROM adrestia_games
      INNER JOIN adrestia_rules ON game_rules_id = adrestia_rules.id
    WHERE game_uid = %s
  )sql", work.quote(game_uid).c_str());

  if (search_result.size() == 0) {
    throw string("Could not find game state");
  }
  game_rules = json::parse(search_result[0][0].as<string>());
  last_events.clear();
  if (!search_result[0]["last_events"].is_null()) {
    json last_events_json = json::parse(search_result[0]["last_events"].as<string>());
    for (const json &event : last_events_json) {
      last_events.push_back(event);
    }
  }

  logger.trace("Committing transaction...");
  work.commit();

  return json::parse(search_result[0][1].as<string>());
}


json adrestia_database::matchmake_in_database (
  const Logger& logger,
  pqxx::connection& psql_connection,
  const string& uuid,
  const vector<string>& selected_books
) {
  /* @brief Adds the user's uuid to the list of uuids waiting for matchmaking, if the user's uuid is not already
   *        there. If someone else is waiting for matchmaking, and the user can be matched with them, then
   *        a new game will be made including both players (and the waiter is removed).
   *
   * @param logger: Logger
   * @param psql_connection: The pqxx PostgreSQL connection.
   * @param uuid: The uuid of the user making the matchmaking request.
   * @param selected_books: The books selected by the user making the matchmaking request
   *
   * @exception string: In case of failing to create a game - likely due to being unable to generate a
   *            random game_uid that is not already in use.
   *
   * @returns: A json object containing the key "game_uid". If the game_uid is "", then no game was made
   *           and the given uuid is waiting for matchmaking. Otherwise, the associated value is the game_uid
   *           of the new game that was made.
   */

  logger.trace(
      "matchmake_in_database called with args:"
      "    uuid: |%s|",
      uuid.c_str());

  pqxx::work work(psql_connection);

  // Check if there are any waiters...
  auto search_result = run_query(logger, work,
    R"sql(
      SELECT uuid, selected_books
      FROM adrestia_match_waiters
      WHERE uuid != %s
      LIMIT 1
      FOR UPDATE
    )sql",
    work.quote(uuid).c_str());

  if (search_result.size() == 0) {
    logger.info("No possible matches are waiting. We shall become a waiter.");

    run_query(logger, work,
      R"sql(
        INSERT INTO adrestia_match_waiters (uuid, selected_books)
        VALUES (%s, %s)
        ON CONFLICT (uuid) DO UPDATE SET selected_books = %s
      )sql",
      work.quote(uuid).c_str(), vector_to_sql_array(work, selected_books).c_str(),
      vector_to_sql_array(work, selected_books).c_str());

    logger.trace("Committing transaction.");
    work.commit();

    json return_var;
    return_var["game_uid"] = "";
    return return_var;
  }

  string waiting_uuid = search_result[0]["uuid"].as<string>();
  logger.debug("uuid |%s| is a matching waiter; we will form a new game with them.", waiting_uuid.c_str());

  // Create game id
  string game_uid = "";
  for (int i = 0; i < 1000; i += 1) {
    game_uid = adrestia_hexy::hex_urandom(adrestia_database::GAME_UID_LENGTH);

    auto game_uid_search_result = run_query(logger, work, R"sql(
      SELECT 1 FROM adrestia_games WHERE game_uid = %s
    )sql", work.quote(game_uid).c_str());

    if (game_uid_search_result.size() > 0) {
      continue;
    }
    break;
  }
  if (game_uid.compare("") == 0) {
    logger.error("Failed to create unique game id!");
    throw string("Failed to create unique game id!");
  }
  
  // Get the latest rules
  auto rules_result = run_query(logger, work, R"sql(
    SELECT game_rules FROM adrestia_rules ORDER BY -id LIMIT 1
    )sql");
  if (rules_result.size() == 0) {
    throw string("No game rules records in database");
  }
  GameRules rules = json::parse(rules_result[0][0].as<string>());

  // Get the other player's selected books
  vector<string> waiting_selected_books = sql_array_to_vector(search_result[0]["selected_books"].as<string>());

  // Create the game
  size_t creator_player_id = 0; // TODO: charles: Randomize this, though it shouldn't matter
  vector<vector<string>> books = { selected_books, waiting_selected_books };
  if (creator_player_id == 1) {
    swap(books[0], books[1]);
  }
  GameState game_state(rules, books);

  // Insert the game into the database
  logger.info("Creating game |%s|", game_uid.c_str());
  run_query(logger, work, R"sql(
    INSERT INTO adrestia_games (game_uid, creator_uuid, activity_state, game_state, game_rules_id)
    SELECT %s, %s, 0, %s, id
    FROM adrestia_rules ORDER BY -id LIMIT 1
  )sql",
      work.quote(game_uid).c_str(),
      work.quote(uuid).c_str(),
      work.quote(json(game_state).dump()).c_str());
  run_query(logger, work, R"sql(
    INSERT INTO adrestia_players (game_uid, user_uid, player_id, player_state, last_move_time)
    VALUES (%s, %s, %d, 0, NOW())
  )sql",
      work.quote(game_uid).c_str(),
      work.quote(uuid).c_str(),
      creator_player_id);
  run_query(logger, work, R"sql(
    INSERT INTO adrestia_players (game_uid, user_uid, player_id, player_state, last_move_time)
    VALUES (%s, %s, %d, 0, NOW())
  )sql",
      work.quote(game_uid).c_str(),
      work.quote(waiting_uuid).c_str(),
      1 - creator_player_id);

  // Remove waiting uuid
  logger.debug("Removing waiting uuid |%s|", waiting_uuid.c_str());
  run_query(logger, work, "DELETE FROM adrestia_match_waiters WHERE uuid = %s", work.quote(waiting_uuid).c_str());

  // Commit
  logger.trace("Committing transaction...");
  work.commit();

  json return_var;
  return_var["game_uid"] = game_uid;
  return return_var;
}

bool adrestia_database::submit_move_in_database(
  const Logger& logger,
  pqxx::connection& psql_connection,
  const string& uuid,
  const string& game_uid,
  const vector<string>& player_move
) {
  /* @brief Submits the move for a turn in the game, updating the game if all
    * players have submitted a move.
    *
    * @param logger: Logger
    * @param psql_connection: The pqxx PostgreSQL connection.
    * @param uuid: The uuid whose user_name is to be altered.
    * @param game_uid: The uid of the game to submit the move for
    * @param player_move: The player's move to submit
    *
    * @returns: Whether the move was legal
    */

  pqxx::work work(psql_connection);

  // Stage 1: submit the move
  logger.trace("submitting move in database");

  // Verify that the player is in that game, and get the game rules and game
  // state and player number and state
  auto result = run_query(logger, work, R"sql(
    SELECT game_rules, game_state, activity_state, player_id, player_state
    FROM
      adrestia_rules
      INNER JOIN adrestia_games ON adrestia_rules.id = game_rules_id
      INNER JOIN adrestia_players ON adrestia_games.game_uid = adrestia_players.game_uid
    WHERE adrestia_players.game_uid = %s
      AND adrestia_players.user_uid = %s
    )sql",
      work.quote(game_uid).c_str(),
      work.quote(uuid).c_str());

  if (result.size() == 0) {
    // That player isn't in that game
    logger.error(
        "Player |%s| submitted a move for game |%s|, but they aren't in that game.",
        uuid.c_str(), game_uid.c_str());
    return false;
  }

  if (result[0]["activity_state"].as<int>() != 0) {
    // The game is not active
    logger.error(
        "Player |%s| submitted a move for inactive game |%s|.",
        uuid.c_str(), game_uid.c_str());
    return false;
  }

  if (result[0]["player_state"].as<int>() != 0) {
    // The player has already submitted a move
    logger.error(
        "Player |%s| resubmitted a move for game |%s|.",
        uuid.c_str(), game_uid.c_str());
    return false;
  }

  // Construct the game rules and game state
  GameRules game_rules = json::parse(result[0]["game_rules"].as<string>());
  GameState game_state(game_rules, json::parse(result[0]["game_state"].as<string>()));

  size_t player_id = result[0]["player_id"].as<int>();
  if (!game_state.is_valid_action(player_id, player_move)) {
    // Illegal action
    logger.error(
        "Player |%s| submitted an illegal move |%s| for game |%s|.",
        uuid.c_str(), json(player_move).dump().c_str(), game_uid.c_str());
    return false;
  }

  // The move is very cool and very legal, add it to the database and update the player's state.
  run_query(logger, work, R"sql(
    UPDATE adrestia_players
    SET player_state = 1, player_move = %s, last_move_time = NOW()
    WHERE game_uid = %s
      AND user_uid = %s
  )sql",
    work.quote(json(player_move).dump()).c_str(),
    work.quote(game_uid).c_str(),
    work.quote(uuid).c_str());

  // Commit
  logger.trace("Committing transaction...");
  work.commit();

  // Start a new transaction
  pqxx::work work_sim(psql_connection);
  result = run_query(logger, work_sim, R"sql(
    SELECT player_move
    FROM
      adrestia_players
    WHERE adrestia_players.game_uid = %s
      AND player_state = 1
    ORDER BY player_id
    FOR UPDATE
  )sql",
      work_sim.quote(game_uid).c_str());

  if (result.size() != 2) {
    // Not all players are ready, or the other player submitted at the same
    // time and their babysitter won the race to update.
    return true;
  }

  logger.trace("Simulating the game update...");

  vector<vector<string>> actions;
  for (const auto &player_move : result) {
    json player_move_json = json::parse(player_move[0].as<string>());
    vector<string> v;
    for (const string &spell : player_move_json) v.push_back(spell);
    actions.push_back(v);
  }
  vector<json> events;
  bool simulated = game_state.simulate(actions, events);

  if (!simulated) {
    // This should really never happen.
    logger.error(
        "An error occurred while simulating a game update for game |%s|",
        game_uid.c_str());
    return true;
  }

  bool concluded = game_state.winners().size() > 0;

  // Update the game with the new state
  run_query(logger, work_sim, R"sql(
    UPDATE adrestia_games
    SET activity_state = %d,
        game_state = %s,
        last_events = %s
    WHERE game_uid = %s
  )sql",
      concluded ? 1 : 0,
      work_sim.quote(json(game_state).dump()).c_str(),
      work_sim.quote(json(events).dump()).c_str(),
      work_sim.quote(game_uid).c_str());
  // Update the players
  run_query(logger, work_sim, R"sql(
    UPDATE adrestia_players
    SET player_state = 0,
        player_move = NULL
    WHERE game_uid = %s
  )sql",
      work_sim.quote(game_uid).c_str());

  // Commit
  logger.trace("Committing transaction...");
  work_sim.commit();

  if (concluded) {
    int game_result = -2;
    if (game_state.winners().size() == 1) {
      game_result = game_state.winners()[0] == player_id;
    }
    conclude_game_in_database(logger, psql_connection, game_uid, uuid, game_result);
  }

  return true;
}


std::vector<std::string> adrestia_database::get_notifications(
  const Logger& logger,
  pqxx::connection& psql_connection,
  const std::string& uuid,
  int &latest_notification_already_sent
) {
  /* @brief Returns a list of messages and updates
   * latest_notification_already_sent. */

  logger.trace_()
    << "get_notifications called with args:\n"
    << "    uuid: |" << uuid << "|\n"
    << "    latest_notification_already_sent: |" << latest_notification_already_sent << "|"
    << endl;

  std::vector<std::string> result;

  pqxx::work work(psql_connection);
  pqxx::result search_results = run_query(logger, work, R"sql(
  SELECT id, message
    FROM adrestia_notifications
    WHERE (target_uuid = %s OR target_uuid = '*')
      AND id > %d
      ORDER BY id
  )sql", work.quote(uuid).c_str(), latest_notification_already_sent);

  for (const auto &search_result : search_results) {
    result.push_back(search_result["message"].as<string>());
    latest_notification_already_sent =
      std::max(latest_notification_already_sent, search_result["id"].as<int>());
  }

  return result;
}


pqxx::connection adrestia_database::establish_connection() {
  const char *db_conn_string = getenv("DB_CONNECTION_STRING");
  if (db_conn_string == nullptr) {
    cerr << "Failed to read DB_CONNECTION_STRING from env." << endl;
    throw string("Failed to read DB_CONNECTION_STRING from env.");
  }
  return pqxx::connection(db_conn_string);
}
