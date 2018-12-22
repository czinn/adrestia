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
	int handle_change_user_name(const json& client_json, json& resp);
	int handle_register_new_account(const json& client_json, json& resp);
	int handle_authenticate(const json& client_json, json& resp);
}