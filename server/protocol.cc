// File containing client-to-server calls

// Us
#include "adrestia_networking.h"
#include "../cpp/game_rules.h"

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
  const std::string& password,
  bool debug,
  const std::string& user_name,
  const std::string& platform
) {
  client_json[adrestia_networking::HANDLER_KEY] = "register_new_account";
  client_json["password"] = password;
  client_json["debug"] = debug;
  client_json["user_name"] = user_name;
  client_json["platform"] = platform;
}


void adrestia_networking::create_authenticate_call(json& client_json,
  const std::string& uuid,
  const std::string& password
) {
  client_json[adrestia_networking::HANDLER_KEY] = "authenticate";
  client_json["uuid"] = uuid;
  client_json["password"] = password;
}


void adrestia_networking::create_abort_game_call(json& client_json,
  const std::string& game_uid
) {
  client_json[adrestia_networking::HANDLER_KEY] = "abort_game";
  client_json["game_uid"] = game_uid;
}

void adrestia_networking::create_change_user_name_call(json& client_json,
  const std::string& user_name
) {
  client_json[adrestia_networking::HANDLER_KEY] = "change_user_name";
  client_json["user_name"] = user_name;
}


void adrestia_networking::create_matchmake_me_call(json& client_json,
  const GameRules &game_rules,
  const std::vector<std::string>& selected_books
) {
  client_json[adrestia_networking::HANDLER_KEY] = "matchmake_me";
  client_json["game_rules"] = json(game_rules);
  client_json["selected_books"] = selected_books;
}

void adrestia_networking::create_submit_move_call(json& client_json,
    const std::string& game_uid,
    const std::vector<std::string>& player_move
) {
  client_json[adrestia_networking::HANDLER_KEY] = "submit_move";
  client_json["game_uid"] = game_uid;
  client_json["player_move"] = player_move;
}

void adrestia_networking::create_get_stats_call(json& client_json) {
  client_json[adrestia_networking::HANDLER_KEY] = "get_stats";
}

void adrestia_networking::create_deactivate_account_call(json& client_json) {
  client_json[adrestia_networking::HANDLER_KEY] = "deactivate_account";
}

void adrestia_networking::create_get_user_profile_call(json& client_json,
  const std::string& friend_code
) {
  client_json[adrestia_networking::HANDLER_KEY] = "get_user_profile";
  client_json["friend_code"] = friend_code;
}

void adrestia_networking::create_follow_user_call(json& client_json,
  const std::string& friend_code
) {
  client_json[adrestia_networking::HANDLER_KEY] = "follow_user";
  client_json["friend_code"] = friend_code;
}

void adrestia_networking::create_unfollow_user_call(json& client_json,
  const std::string& friend_code
) {
  client_json[adrestia_networking::HANDLER_KEY] = "unfollow_user";
  client_json["friend_code"] = friend_code;
}
