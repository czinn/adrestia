// File containing client-to-server calls

// Us
#include "adrestia_networking.h"

// System modules
#include <string>

// JSON
#include "../units_cpp/json.h"
using json = nlohmann::json;


void adrestia_networking::create_floop_call(json& client_json) {
	client_json[adrestia_networking::HANDLER_KEY] = "floop";
}


void adrestia_networking::create_establish_connection_call(json& client_json) {
	client_json[adrestia_networking::HANDLER_KEY] = "establish_connection";
}


void adrestia_networking::create_register_new_account_call(json& client_json,
	const std::string& password
) {
	client_json[adrestia_networking::HANDLER_KEY] = "register_new_account";
	client_json["password"] = password;
}


void adrestia_networking::create_authenticate_call(json& client_json,
	const std::string& uuid,
	const std::string& password
) {
	client_json[adrestia_networking::HANDLER_KEY] = "authenticate";
	client_json["uuid"] = uuid;
	client_json["password"] = password;
}


void adrestia_networking::create_change_user_name_call(json& client_json,
	const std::string& user_name
) {
	client_json[adrestia_networking::HANDLER_KEY] = "change_user_name";
	client_json["user_name"] = user_name;
}

