// File containing client-to-server calls

// Us
#include "adrestia_networking.h"

// System modules
#include <string>

// JSON
#include "../cpp/json.h"
using json = nlohmann::json;


void adrestia_networking::create_floop_call(json& client_json) {
  client_json[adrestia_networking::HANDLER_KEY] = "floop";
}


void adrestia_networking::create_establish_connection_call(json& client_json, const std::string &version) {
  client_json[adrestia_networking::HANDLER_KEY] = "establish_connection";
	client_json["client_version"] = version;
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


void adrestia_networking::create_matchmake_me_call(json& client_json,
  const std::vector<std::string>& selected_books
) {
  client_json[adrestia_networking::HANDLER_KEY] = "matchmake_me";
  client_json["selected_books"] = selected_books;
}

