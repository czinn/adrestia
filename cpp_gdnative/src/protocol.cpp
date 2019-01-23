#include "protocol.h"

#include "../../server/adrestia_networking.h"

#define CLASSNAME Protocol

#define FORWARD_API_CALL(name, ...)\
  String Protocol::create_ ## name ## _call() {\
    nlohmann::json j;\
    adrestia_networking::create_ ## name ## _call(j);\
    String s(j.dump().c_str());\
    return s;\
  }

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

  String Protocol::create_establish_connection_call() {
    nlohmann::json j;
    adrestia_networking::create_establish_connection_call(j);
    return String(j.dump().c_str());
  }

  String Protocol::create_authenticate_call(String uuid, String password) {
    nlohmann::json j;
    std::string uuid_; of_godot_variant(uuid, &uuid_);
    std::string password_; of_godot_variant(password, &password_);
    adrestia_networking::create_authenticate_call(j, uuid_, password_);
    return String(j.dump().c_str());
  }

  String Protocol::create_change_user_name_call(String user_name) {
    nlohmann::json j;
    std::string user_name_; of_godot_variant(user_name, &user_name_);
    adrestia_networking::create_change_user_name_call(j, user_name_);
    return String(j.dump().c_str());
  }

  String Protocol::create_matchmake_me_call(Variant selected_books) {
    nlohmann::json j;
    std::vector<std::string> selected_books_; of_godot_variant(selected_books, &selected_books_);
    adrestia_networking::create_matchmake_me_call(j, selected_books_);
    return String(j.dump().c_str());
  }
}
