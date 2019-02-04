/* The database interface component for adrestia's networking. */

// Us
#include "adrestia_database.h"

// Our related modules
#include "adrestia_hexy.h"
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
pqxx::result run_query(pqxx::work& work, const char* query_format, ...) {
  va_list args;

  // First, use [vsnprintf] to figure out how long the query needs to be.
  va_start(args, query_format);
  size_t needed = vsnprintf(nullptr, 0, query_format, args) + 1;
  va_end(args);

  // Then, use [vsprintf] to format it and run it.
  va_start(args, query_format);
  char *query = new char[needed];
  vsprintf(query, query_format, args);
  cout << query << endl;

  pqxx::result result;
  result = work.exec(query);

  delete[] query;
  va_end(args);
  return result;
}


string hash_password(const string& password, const string& salt) {
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
    const string& log_id,
    pqxx::connection* psql_connection,
    int id
) {
  /* @brief Gets the game rules with the given id
   *
   * @param log_id: A string prepended to log lines
   * @param psql_connection: The pqxx PostgreSQL connection.
   * @param id: Id of the game rules, from the game table. An id of 0 gets the
   *            most recent set of rules.
   *
   * @returns: A GameRules object constructed from the database record
   */

  cout << "[" << log_id << "] retrieve_game_rules called with args:" << endl
       << "    id: |" << id << "|" << endl;

  pqxx::work work(*psql_connection);

  if (id == 0) {
    auto search_result = run_query(work, R"sql(
      SELECT game_rules FROM adrestia_rules ORDER BY -id LIMIT 1
      )sql");
    if (search_result.size() == 0) {
      throw string("No game rules records in database");
    }
    return json::parse(search_result[0][0].as<string>());
  } else {
    auto search_result = run_query(work, R"sql(
      SELECT game_rules FROM adrestia_rules WHERE id = %d
      )sql", id);
    if (search_result.size() == 0) {
      throw string("Could not find game rules");
    }
    return json::parse(search_result[0][0].as<string>());
  }
}

json adrestia_database::retrieve_gamestate_from_database (
  const string& log_id,
  pqxx::connection* psql_connection,
  const string& game_uid,
  GameRules &game_rules
) {
  /* @brief Retrieves the gamestate of a target game.
   *
   * @param log_id: A string prepended to log lines
   * @param psql_connection: The pqxx PostgreSQL connection.
   * @param game_uid: The game_uid whose game_state will be returned.
   *                  If there is no such game, or the game has no state,
   *                      this will be an empty string.
   * @param game_rules: A GameRules object that should be populated with the
   * game's rules.
   * @returns: json representation of the game state
   */

  cout << "[" << log_id << "] retrieve_gamestate_from_database called with args:" << endl
       << "    game_uid: |" << game_uid << "|" << endl;

  pqxx::work work(*psql_connection);

  auto search_result = run_query(work, R"sql(
    SELECT game_rules, game_state
    FROM adrestia_games
      INNER JOIN adrestia_rules ON game_rules_id = adrestia_rules.id
    WHERE game_uid = %s
  )sql", work.quote(game_uid).c_str());

  if (search_result.size() == 0) {
    throw string("Could not find game state");
  }
  game_rules = json::parse(search_result[0][0].as<string>());
  return json::parse(search_result[0][1].as<string>());
}


json adrestia_database::check_for_active_games_in_database (
  const string& log_id,
  pqxx::connection* psql_connection,
  const string& uuid
) {
  /* @brief Checks the database to see if the given user is part of any active games, and returns the game_uids of
   *        those games. Also returns which games are waiting for this uuid to make a move.
   *
   * @param log_id: A string prepended to log lines
   * @param psql_connection: The pqxx PostgreSQL connection.
   * @param uuid: We will be looking for games involving this uuid.
   *
   * @returns: A json object with the following tags:
   *               "active_game_uids": An array of active game_uids that the given uuid is part of
   *               "waiting_game_uids": An array of active game_uids that are waiting for the given uuid to make a
   *                                    move. This is, of course, a subset of active_game_uids.
   */

  cout << "[" << log_id << "] check_for_active_games_in_database called with args:" << endl
       << "    uuid: |" << uuid << "|" << endl;

  pqxx::work work(*psql_connection);

  auto search_result = run_query(work, R"sql(
    SELECT adrestia_games.game_uid, player_state
    FROM adrestia_games
      INNER JOIN adrestia_players ON adrestia_games.game_uid = adrestia_players.game_uid
    WHERE activity_state = 0
      AND user_uid = %s
  )sql", work.quote(uuid).c_str());

  vector<string> active_game_uids;
  vector<string> waiting_game_uids;

  for (const auto &result : search_result) {
    // This is an active game that we are in.
    string game_uid = result["game_uid"].as<string>();
    active_game_uids.push_back(game_uid);

    cout << "[" << log_id << "] uuid |" << uuid << "| has active game |" << game_uid << "|." << endl;

    // If the current player_state is 0...
    if (result["player_state"].as<int>() == 0) {
      // We need to make a move.
      waiting_game_uids.push_back(game_uid);
      cout << "[" << log_id << "] Waiting for uuid |" << uuid << "| to make a move in game |" << game_uid << "|..." << endl;
    }
  }

  cout << "[" << log_id << "] Commiting transaction..." << endl;
  work.commit();

  json return_var;
  return_var["active_game_uids"] = active_game_uids;
  return_var["waiting_game_uids"] = waiting_game_uids;

  cout << "[" << log_id << "] check_for_games concluded." << endl;
  return return_var;
}


json adrestia_database::matchmake_in_database (
  const string& log_id,
  pqxx::connection* psql_connection,
  const string& uuid,
  const vector<string>& selected_books
) {
  /* @brief Adds the user's uuid to the list of uuids waiting for matchmaking, if the user's uuid is not already
   *        there. If someone else is waiting for matchmaking, and the user can be matched with them, then
   *        a new game will be made including both players (and the waiter is removed).
   *
   * @param log_id: A string prepended to log lines
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

  cout << "[" << log_id << "] matchmake_in_database called with args:" << endl
       << "    uuid: |" << uuid << "|" << endl;

  pqxx::work work(psql_connection[0]);

  // Check if there are any waiters...
  auto search_result = run_query(work,
    R"sql(
      SELECT uuid, selected_books
      FROM adrestia_match_waiters
      WHERE uuid != %s
      LIMIT 1
      FOR UPDATE
    )sql",
    work.quote(uuid).c_str());

  if (search_result.size() == 0) {
    cout << "[" << log_id << "] No possible matches are waiting. We shall become a waiter." << endl;

    run_query(work, R"sql(
      INSERT INTO adrestia_match_waiters (uuid, selected_books)
      VALUES (%s, %s)
    )sql", work.quote(uuid).c_str(), vector_to_sql_array(work, selected_books).c_str());

    cout << "[" << log_id << "] Commiting transaction." << endl;
    work.commit();

    json return_var;
    return_var["game_uid"] = "";
    return return_var;
  }

  string waiting_uuid = search_result[0]["uuid"].as<string>();
  cout << "[" << log_id << "] uuid |" << waiting_uuid << "| is a matching waiter; we will form a new game with them." << endl;

  // Create game id
  string game_uid = "";
  for (int i = 0; i < 1000; i += 1) {
    game_uid = adrestia_hexy::hex_urandom(adrestia_database::GAME_UID_LENGTH);

    auto game_uid_search_result = run_query(work, R"sql(
      SELECT 1 FROM adrestia_games WHERE game_uid = %s
    )sql", work.quote(game_uid).c_str());

    if (game_uid_search_result.size() > 0) {
      continue;
    }
    break;
  }
  if (game_uid.compare("") == 0) {
    cerr << "[" << log_id << "] Failed to create unique game id!" << endl;
    throw string("Failed to create unique game id!");
  }
  
  // Get the latest rules
  auto rules_result = run_query(work, R"sql(
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
  cout << "[" << log_id << "] Creating game |" << game_uid << "|..." << endl;
  run_query(work, R"sql(
    INSERT INTO adrestia_games (game_uid, creator_uuid, activity_state, game_state, game_rules_id)
    SELECT %s, %s, 0, %s, id
    FROM adrestia_rules ORDER BY -id LIMIT 1
  )sql",
      work.quote(game_uid).c_str(),
      work.quote(uuid).c_str(),
      work.quote(json(game_state).dump()).c_str());
  run_query(work, R"sql(
    INSERT INTO adrestia_players (game_uid, user_uid, player_id, player_state)
    VALUES (%s, %s, %d, 0)
  )sql",
      work.quote(game_uid).c_str(),
      work.quote(uuid).c_str(),
      creator_player_id);
  run_query(work, R"sql(
    INSERT INTO adrestia_players (game_uid, user_uid, player_id, player_state)
    VALUES (%s, %s, %d, 0)
  )sql",
      work.quote(game_uid).c_str(),
      work.quote(waiting_uuid).c_str(),
      1 - creator_player_id);

  // Remove waiting uuid
  cout << "[" << log_id << "] Removing waiting uuid |" << waiting_uuid << "|..." << endl;
  run_query(work, "DELETE FROM adrestia_match_waiters WHERE uuid = %s", work.quote(waiting_uuid).c_str());

  // Commit
  cout << "[" << log_id << "] Committing transaction..." << endl;
  work.commit();

  json return_var;
  return_var["game_uid"] = game_uid;
  return return_var;
}

json adrestia_database::adjust_user_name_in_database(
  const string& log_id,
  pqxx::connection* psql_connection,
  const string& uuid,
  const string& user_name
) {
  /* @brief Changes all accounts in adrestia_accounts with uuid uuid to have user_name user_name.
   *        By necessity, this requires the generation of a random tag to go with the name. 1000 attempts will be
   *        made to generate the tag.
   *
   * @param log_id: A line prepended to diagnostic log lines
   * @param psql_connection: The pqxx PostgreSQL connection.
   * @param uuid: The uuid whose user_name is to be altered.
   * @param user_name: The new user_name for the given uuid.
   *
   * @exception string: In case of failing to update for some reason - likely due to being unable to generate a
   *            random tag that is not already in use with the given user_name.
   *
   * @returns A json object containing "tag": the new tag generated.
   */

  cout << "[" << log_id << "] adjust_user_name_in_database called with args:" << endl
       << "    uuid: |" << uuid << "|" << endl
       << "    user_name: |" << user_name << "|" << endl;

  json new_account_info;

  bool success = false;

  for (int i = 0; i < 1000; i += 1) {
    string tag = adrestia_hexy::hex_urandom(adrestia_database::TAG_LENGTH);

    pqxx::work work(psql_connection[0]);
    try {
      pqxx::result result = run_query(work,
        R"sql(
          UPDATE adrestia_accounts
          SET user_name = %s, tag = %s
          WHERE uuid = %s
        )sql",
        work.quote(user_name).c_str(), work.quote(tag).c_str(),
        work.quote(uuid).c_str());
      work.commit();

      cout << "[" << log_id << "] Successfully adjustment of user_name in database." << endl;
      success = true;
      new_account_info["tag"] = tag;
      break;
    }
    catch (pqxx::integrity_constraint_violation &) {
      work.abort();
      continue;
    }
  }

  if (!success) {
    cout << "[" << log_id << "] Failed to update the user_name!";
    throw string("Failed to update user name of uuid |" + uuid + "| to user_name |" + user_name + "|!");
  }

  return new_account_info;
}


json adrestia_database::register_new_account_in_database(
  const string& log_id,
  pqxx::connection* psql_connection,
  const string& password
) {
  /* @brief Creates a new account in adrestia_accounts with the given password. The account will be given a default
   *        user_name and a random (not-already-in-use-with-this-name) tag to go with it.
   *        1000 attempts will be made to generate a non-conflicting tag.
   *
   * @param log_id: A string prepended to diagnostic log lines
   * @param psql_connection: The pqxx PostgreSQL connection.
   * @param password: The password that will be used with this new account.
   *
   * @exception string: In the case of failing to generate a non-conflicting tag after 1000 attempts, though possibly
   *            also in the case of other database integrity constraint violations.
   *
   * @returns: A json object with:
   *               "id": The uuid of the new account
   *               "user_name": The user_name of the new account
   *               "tag": The tag of the new account
   */

  const string default_user_name = "Guest";

  cout << "[" << log_id << "] register_new_account_in_database called with args:" << endl
       << "    password: |" << password << "|" << endl;

  string salt = adrestia_hexy::hex_urandom(adrestia_database::SALT_LENGTH);
  pqxx::binarystring password_hash(hash_password(password, salt));

  json new_account;
  bool actually_created_account = false;

  // Keep making up ids/tags until we get a successful insertion.
  for (int i = 0; i < 1000; i += 1) {
    string uuid = adrestia_hexy::hex_urandom(adrestia_database::UUID_LENGTH);
    string tag = adrestia_hexy::hex_urandom(adrestia_database::TAG_LENGTH);

    pqxx::work work(psql_connection[0]);
    try {
      run_query(work, R"sql(
        INSERT INTO adrestia_accounts (uuid, user_name, tag, hash_of_salt_and_password, salt)
        VALUES (%s, %s, %s, %s, %s)
      )sql",
          work.quote(uuid).c_str(),
          work.quote(default_user_name).c_str(),
          work.quote(tag).c_str(),
          work.quote(password_hash).c_str(),
          work.quote(salt).c_str());
      work.commit();

      cout << "[" << log_id << "] Successfully finished insertion of new account into database." << endl;
      actually_created_account = true;
      new_account["uuid"] = uuid;
      new_account["tag"] = tag;
      new_account["user_name"] = default_user_name;
      break;
    }
    catch (pqxx::integrity_constraint_violation &) {
      work.abort();
      continue;
    }
  }

  if (!actually_created_account) {
    cerr << "[" << log_id << "] Failed to create a non-conflicting uuid/tag pair!" << endl;
    throw string("Failed to generate non-conflicting uuid/tag pair.");
  }

  return new_account;
}


json adrestia_database::verify_existing_account_in_database(
  const string& log_id,
  pqxx::connection* psql_connection,
  const string& uuid,
  const string& password
) {
  /* @brief Returns json {'success': bool, 'user_name': string, 'tag': string } */

  cout << "[" << log_id << "] verify_existing_account_in_database called with args:" << endl
       << "    uuid: |" << uuid << "|" << endl
       << "    password: |" << password << "|" << endl;
  
  json result;

  pqxx::work work(*psql_connection);
  pqxx::result search_result = run_query(work, R"sql(
  SELECT hash_of_salt_and_password, salt, user_name, tag
    FROM adrestia_accounts
    WHERE uuid = %s
  )sql", work.quote(uuid).c_str());

  // Find account of given name
  if (search_result.size() == 0) {
    cout << "[" << log_id << "] This uuid not found in database.";
    result["valid"] = false;
    return result;
  }

  pqxx::binarystring expected_hash(search_result[0]["hash_of_salt_and_password"]);
  string salt = search_result[0]["salt"].as<string>();
  pqxx::binarystring actual_hash(hash_password(password, salt));

  if (actual_hash == expected_hash) {
    cout << "[" << log_id << "] This uuid and password have been verified." << endl;
    result["valid"] = true;
    result["user_name"] = search_result[0]["user_name"].as<string>();
    result["tag"] = search_result[0]["tag"].as<string>();
    return result;
  }

  cout << "[" << log_id << "] Received an incorrect password for this known uuid." << endl;
  result["valid"] = false;
  return result;
}


std::vector<std::string> adrestia_database::get_notifications(
  const std::string& log_id,
  pqxx::connection* psql_connection,
  const std::string& uuid,
  int &latest_notification_already_sent
) {
  /* @brief Returns a list of messages and updates
   * latest_notification_already_sent. */

  cout << "[" << log_id << "] get_notifications called with args:" << endl
       << "    uuid: |" << uuid << "|" << endl
       << "    latest_notification_already_sent: |"
       << latest_notification_already_sent << "|" << endl;

  std::vector<std::string> result;

  pqxx::work work(*psql_connection);
  pqxx::result search_results = run_query(work, R"sql(
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


pqxx::connection* adrestia_database::establish_psql_connection() {
  /* Returns a pointer to a pqxx::connection in the heap.
   * Connection parameters specified via environment variable (DB_CONNECTION_STRING)
   */

  const char *db_conn_string = getenv("DB_CONNECTION_STRING");
  if (db_conn_string == nullptr) {
    cerr << "Failed to read DB_CONNECTION_STRING from env." << endl;
    throw string("Failed to read DB_CONNECTION_STRING from env.");
  }
  pqxx::connection* psql_connection = new pqxx::connection(db_conn_string);
  return psql_connection;
}
