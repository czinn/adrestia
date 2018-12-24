#pragma once

#include <string>

namespace adrestia_networking {
	const int DEFAULT_SERVER_PORT = 18677;
	const int MESSAGE_MAX_BYTES = 32768;

	const string HANDLER_KEY("server_handler_name");
	const string CODE_KEY("api_code");
	const string MESSAGE_KEY("message");

	typedef std::function<int(const json&, json&)> request_handler;

	// Server-side handlers
	int handle_floop(const json& client_json, json& resp);

	int handle_establish_connection(const json& client_json, json& resp);

	int handle_register_new_account(const json& client_json, json& resp);
	int handle_authenticate(const json& client_json, json& resp);

	int handle_change_user_name(const json& client_json, json& resp);

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
}
