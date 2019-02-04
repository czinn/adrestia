#include "protocol.h"

#include "game_rules.h"

#include "../../server/adrestia_networking.h"

#define CLASSNAME Protocol

using namespace godot;

namespace godot {
  Protocol::Protocol() {
  }

  Protocol::~Protocol() {
  }

  void Protocol::_register_methods() {
    REGISTER_METHOD(create_floop_call);
    REGISTER_METHOD(create_establish_connection_call);
    REGISTER_METHOD(create_register_new_account_call);
    REGISTER_METHOD(create_authenticate_call);
    REGISTER_METHOD(create_change_user_name_call);
    REGISTER_METHOD(create_matchmake_me_call);
  }

  String Protocol::create_floop_call() {
    nlohmann::json j;
    adrestia_networking::create_floop_call(j);
    return String(j.dump().c_str());
  }

  String Protocol::create_establish_connection_call(String version) {
    nlohmann::json j;
    std::string version_; of_godot_variant(version, &version_);
    adrestia_networking::create_establish_connection_call(j, version_);
    return String(j.dump().c_str());
  }

  String Protocol::create_authenticate_call(String uuid, String password) {
    nlohmann::json j;
    std::string uuid_; of_godot_variant(uuid, &uuid_);
    std::string password_; of_godot_variant(password, &password_);
    adrestia_networking::create_authenticate_call(j, uuid_, password_);
    return String(j.dump().c_str());
  }

  String Protocol::create_register_new_account_call(String password) {
    nlohmann::json j;
    std::string password_; of_godot_variant(password, &password_);
    adrestia_networking::create_register_new_account_call(j, password_);
    return String(j.dump().c_str());
  }

  String Protocol::create_change_user_name_call(String user_name) {
    nlohmann::json j;
    std::string user_name_; of_godot_variant(user_name, &user_name_);
    adrestia_networking::create_change_user_name_call(j, user_name_);
    return String(j.dump().c_str());
  }

  String Protocol::create_matchmake_me_call(Variant rules, Variant selected_books) {
    nlohmann::json j;
    std::vector<std::string> selected_books_; of_godot_variant(selected_books, &selected_books_);
    auto *_rules = godot::as<GameRules>(rules);
    adrestia_networking::create_matchmake_me_call(j, *_rules->_ptr, selected_books_);
    return String(j.dump().c_str());
  }
}
