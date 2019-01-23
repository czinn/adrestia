#ifndef ADRESTIA_NETWORKING_INCLUDE_GUARD
#define ADRESTIA_NETWORKING_INCLUDE_GUARD

// System modules
#include <map>
#include <string>

// JSON
#include "../cpp/json.h"
using json = nlohmann::json;


namespace adrestia_networking {
  const int DEFAULT_SERVER_PORT = 18677;
  const int MESSAGE_MAX_BYTES = 32768;

  const time_t WAIT_FOR_COMMANDS_SECONDS = 10;  // We wait for commands for this long
  const time_t TIMEOUT_SEND_SECONDS = 30;  // When sending data to client, we assume connection dropped after this long

  const std::string HANDLER_KEY("api_handler_name");
  const std::string CODE_KEY("api_code");
  const std::string MESSAGE_KEY("api_message");

  typedef std::function<int(const std::string&, const json&, json&)> request_handler;

  // Server functions
  std::string read_message(int client_socket, bool& timed_out);
  void babysit_client(int server_socket, int client_socket);
  void listen_for_connections(int port);

  // Server-side handlers
  int handle_floop(const std::string& log_id,
                   const json& client_json,
                   json& resp
                  );

  int handle_establish_connection(const std::string& log_id,
                                  const json& client_json,
                                  json& resp
                                 );

  int handle_register_new_account(const std::string& log_id,
                                  const json& client_json,
                                  json& resp
                                 );
  int handle_authenticate(const std::string& log_id,
                          const json& client_json,
                          json& resp
                         );

  int handle_change_user_name(const std::string& log_id,
                              const json& client_json,
                              json& resp
                             );
  int handle_matchmake_me(const std::string& log_id,
                          const json& client_json,
                          json& resp
                         );

  // Calls to handlers
  void create_floop_call(json& client_json);

  void create_establish_connection_call(json& client_json);

  void create_register_new_account_call(json& client_json,
                                        const std::string& password
                                       );
  void create_authenticate_call(json& client_json,
                                const std::string& uuid,
                                const std::string& password
                               );

  void create_change_user_name_call(json& client_json,
                                    const std::string& user_name
                                   );
  void create_matchmake_me_call(json& client_json,
                                const std::vector<std::string>& selected_books
                               );

  // Server-side pushers
  void push_active_games(const std::string& log_id,
                         json& message_json,
                         const std::string& uuid,
                         std::map<std::string, std::string>& games_I_am_aware_of,
                         std::vector<std::string>& active_game_uids_I_am_aware_of
                        );
}

#endif
