/* The database interface component for adrestia's networking. */

#ifndef ADRESTIA_DATABASE_INCLUDE_GUARD
#define ADRESTIA_DATABASE_INCLUDE_GUARD

// Our related modules
#include "../cpp/game_rules.h"
#include "../cpp/game_state.h"
#include "logger.h"

// Database modules
#include <pqxx/pqxx>

// System modules
#include <string>
#include <vector>

// JSON
#include "../cpp/json.h"
using json = nlohmann::json;

namespace adrestia_database {

  const int SALT_LENGTH = 16;
  const int TAG_LENGTH = 8;
  const int UUID_LENGTH = 32;
  const int GAME_UID_LENGTH = 32;

  /* Marks the target uuid as winning/losing the target game */
  void conclude_game_in_database(
    const Logger& logger,
    pqxx::connection& psql_connection,
    const std::string& game_uid,
    const std::string& uuid,
    int game_result
  );

  /* Gets game rules from the database. */
  GameRules retrieve_game_rules(
    const Logger& logger,
    pqxx::connection& psql_connection,
    int id
  );

  /* Fetches player_id, player_state and player_move for a particular game_uid/uuid pair. */
  json retrieve_player_info_from_database(
    const Logger& logger,
    pqxx::connection& psql_connection,
    const std::string& game_uid,
    const std::string& uuid
  );

  /* Fetches from the database the game_state associated with the given game_uid. */
  json retrieve_gamestate_from_database(
    const Logger& logger,
    pqxx::connection& psql_connection,
    const std::string& game_uid,
    GameRules &game_rules,
    std::vector<json> &last_events
  );

  /* Checks for active games in the database associated with the given uuid, and returns them.
   *     Also returns if any are waiting for this player's move in particular.
   */
  json check_for_active_games_in_database (
    const Logger& logger,
    pqxx::connection& psql_connection,
    const std::string& uuid
  );

  /* Adds the user to the waiting list if there are no compatible waiters;
   *     otherwise, matches the user to a waiter.
   */
  json matchmake_in_database(
    const Logger& logger,
    pqxx::connection& psql_connection,
    const std::string& uuid,
    const std::vector<std::string>& selected_books
  );

  /* Submits the move for a turn in the game, updating the game if all players
   * have submitted a move.
   */
  bool submit_move_in_database(
    const Logger& logger,
    pqxx::connection& psql_connection,
    const std::string& uuid,
    const std::string& game_uid,
    const std::vector<std::string>& player_move
  );

  /* Changes the user_name associated with the given uuid in the database.
   * Returns a json object with key 'tag' representing the tag.
   */
  json adjust_user_name_in_database(
    const Logger& logger,
    pqxx::connection& psql_connection,
    const std::string& uuid,
    const std::string& user_name
  );


  /* Creates a new account with a default name in the database.
   * Returns a json object with keys 'id', 'user_name', and 'tag'.
   */
  json register_new_account_in_database(
    const Logger& logger,
    pqxx::connection& psql_connection,
    const std::string& password
  );


  /* Returns json with keys 'valid', 'user_name', 'tag'.
   */
  json verify_existing_account_in_database(
    const Logger& logger,
    pqxx::connection& psql_connection,
    const std::string& uuid,
    const std::string& password
  );

  /* Returns a list of messages to send to the user, and updates
   * latest_notification_already_sent to the maximum of the IDs of the messages
   * returned. */
  std::vector<std::string> get_notifications(
    const Logger& logger,
    pqxx::connection& psql_connection,
    const std::string& uuid,
    int &latest_notification_already_sent
  );

  /* Returns an established pqxx::connection object,
   *     connection parameters specified via environment variable.
   */
  pqxx::connection establish_connection();

  /* Db and DbQuery form an alternate way to access the database. They provide
   * a flexible, concise interface that allows for the quick development of
   * handlers. */
  class DbQuery {
    public:
      DbQuery(std::string format, pqxx::work *work, Logger &logger);

      template<typename T>
      DbQuery &operator()(const T &x) {
        replace_one_qmark(work->quote(x));
        return *this;
      }
      pqxx::result operator()();
    private:
      std::string format;
      pqxx::work *work;
      Logger &logger;
      void replace_one_qmark(std::string s);
  };

  class Db {
    public:
      Db(Logger &logger);
      ~Db();
      DbQuery query(std::string format);
      void commit();
    private:
      Logger &logger;
      pqxx::connection *conn;
      pqxx::work *work;
  };
}

#endif
