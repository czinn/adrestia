#ifndef ADRESTIA_NETWORKING_INCLUDE_GUARD
#define ADRESTIA_NETWORKING_INCLUDE_GUARD

#include "versioning.h"
#include "../cpp/game_rules.h"
#include "pushers/push_active_games.h"
#include "pushers/push_notifications.h"
#include "logger.h"

// System modules
#include <map>
#include <string>

// JSON
#include "../cpp/json.h"
using json = nlohmann::json;


namespace adrestia_networking {
  const Version SERVER_VERSION = { 1, 0, 0 };
  const Version CLIENT_VERSION = { 2, 0, 0 };
  const int DEFAULT_SERVER_PORT = 18677;
  const int MESSAGE_MAX_BYTES = 32768;

  const time_t WAIT_FOR_COMMANDS_SECONDS = 10;  // We wait for commands for this long
  const time_t TIMEOUT_SEND_SECONDS = 30;  // When sending data to client, we assume connection dropped after this long

  const std::string HANDLER_KEY("api_handler_name");
  const std::string CODE_KEY("api_code");
  const std::string MESSAGE_KEY("api_message");

  typedef std::function<int(const Logger&, const json&, json&)> request_handler;

  // Server functions
  std::string read_message(int client_socket, bool& timed_out);
  void babysit_client(int server_socket, int client_socket);
  void listen_for_connections(int port);

  // Server-side handlers
#define DEF_HANDLER(name)\
  int name(\
      const Logger& logger,\
      const json& client_json,\
      json& resp\
  );

  DEF_HANDLER(handle_floop);
  DEF_HANDLER(handle_establish_connection);
  DEF_HANDLER(handle_register_new_account);
  DEF_HANDLER(handle_authenticate);
  DEF_HANDLER(handle_abort_game);
  DEF_HANDLER(handle_change_user_name);
  DEF_HANDLER(handle_matchmake_me);
  DEF_HANDLER(handle_submit_move);
  DEF_HANDLER(handle_get_stats);
  DEF_HANDLER(handle_deactivate_account);
  DEF_HANDLER(handle_get_user_profile);
  DEF_HANDLER(handle_follow_user);
  DEF_HANDLER(handle_unfollow_user);

  // Calls to handlers
  void create_floop_call(json& client_json);

  void create_establish_connection_call(
      json& client_json,
      const std::string &version
  );

  void create_register_new_account_call(
      json& client_json,
      const std::string& password,
      bool debug
  );

  void create_authenticate_call(
      json& client_json,
      const std::string& uuid,
      const std::string& password
  );

  void create_abort_game_call(
      json& client_json,
      const std::string& game_uid
  );

  void create_change_user_name_call(
      json& client_json,
      const std::string& user_name
  );

  void create_matchmake_me_call(
      json& client_json,
      const GameRules &rules,
      const std::vector<std::string>& selected_books
  );

  void create_submit_move_call(
      json& client_json,
      const std::string& game_uid,
      const std::vector<std::string>& player_move
  );

  void create_get_stats_call(json& client_json);

  void create_deactivate_account_call(json& client_json);

  void create_get_user_profile_call(
      json& client_json,
      const std::string& uuid
  );

  void create_follow_user_call(
      json& client_json,
      const std::string& uuid
  );

  void create_unfollow_user_call(
      json& client_json,
      const std::string& uuid
  );
}

#endif
