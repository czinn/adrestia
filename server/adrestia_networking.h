#ifndef ADRESTIA_NETWORKING_INCLUDE_GUARD
#define ADRESTIA_NETWORKING_INCLUDE_GUARD

// System modules
#include <string>

// JSON
#include "../units_cpp/json.h"
using json = nlohmann::json;


namespace adrestia_networking {
	const int DEFAULT_SERVER_PORT = 18677;
	const int MESSAGE_MAX_BYTES = 32768;

	const time_t WAIT_FOR_COMMANDS_SECONDS = 10;  // We wait for commands for this long
	const time_t TIMEOUT_SEND_SECONDS = 30;  // When sending data to client, we assume connection dropped after this long

	const std::string HANDLER_KEY("server_handler_name");
	const std::string CODE_KEY("api_code");
	const std::string MESSAGE_KEY("api_message");

	typedef std::function<int(const json&, json&)> request_handler;

	// Server functions
	std::string read_message(int client_socket, bool& timed_out);
	void babysit_client(int server_socket, int client_socket);
	void listen_for_connections(int port);

	// Server-side handlers
	int handle_floop(const json& client_json, json& resp);

	int handle_establish_connection(const json& client_json, json& resp);

	int handle_register_new_account(const json& client_json, json& resp);
	int handle_authenticate(const json& client_json, json& resp);

	int handle_change_user_name(const json& client_json, json& resp);
	int matchmake_me(const json& client_json, json& resp);

	// Calls
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
	void create_matchmake_me_call(json& client_json);
}

#endif
