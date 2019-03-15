#include "protocol.h"

#include "game_rules.h"

#include "../../server/adrestia_networking.h"

#define CLASSNAME Protocol

using namespace godot;

#define IMPL_UNIT(fn) \
  String Protocol::fn() {\
    nlohmann::json j;\
    adrestia_networking::fn(j);\
    return String(j.dump().c_str());\
  }

#define IMPL_STRING(fn) \
  String Protocol::fn(String a) {\
    nlohmann::json j;\
    std::string a_; of_godot_variant(a, &a_);\
    adrestia_networking::fn(j, a_);\
    return String(j.dump().c_str());\
  }

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
    REGISTER_METHOD(create_abort_game_call);
    REGISTER_METHOD(create_change_user_name_call);
    REGISTER_METHOD(create_matchmake_me_call);
    REGISTER_METHOD(create_submit_move_call);
    REGISTER_METHOD(create_get_stats_call);
    REGISTER_METHOD(create_deactivate_account_call);
    REGISTER_METHOD(create_get_user_profile_call);
    REGISTER_METHOD(create_follow_user_call);
    REGISTER_METHOD(create_unfollow_user_call);
    REGISTER_METHOD(create_get_friends_call);
  }

  IMPL_UNIT(create_floop_call);
  IMPL_STRING(create_establish_connection_call);

  String Protocol::create_authenticate_call(String uuid, String password) {
    nlohmann::json j;
    std::string uuid_; of_godot_variant(uuid, &uuid_);
    std::string password_; of_godot_variant(password, &password_);
    adrestia_networking::create_authenticate_call(j, uuid_, password_);
    return String(j.dump().c_str());
  }

  IMPL_STRING(create_abort_game_call);

  String Protocol::create_register_new_account_call(String password, bool debug, String user_name, String platform) {
    nlohmann::json j;
    std::string password_; of_godot_variant(password, &password_);
    std::string user_name_; of_godot_variant(user_name, &user_name_);
    std::string platform_; of_godot_variant(platform, &platform_);
    adrestia_networking::create_register_new_account_call(j, password_, debug, user_name_, platform_);
    return String(j.dump().c_str());
  }

  IMPL_STRING(create_change_user_name_call);

  String Protocol::create_matchmake_me_call(Variant rules, Variant selected_books, String target_friend_code) {
    nlohmann::json j;
    std::vector<std::string> selected_books_; of_godot_variant(selected_books, &selected_books_);
    auto *_rules = godot::as<GameRules>(rules);
    std::string target_friend_code_; of_godot_variant(target_friend_code, &target_friend_code_);
    adrestia_networking::create_matchmake_me_call(j, *_rules->_ptr, selected_books_, target_friend_code_);
    return String(j.dump().c_str());
  }

  String Protocol::create_submit_move_call(String game_uid, Variant player_move) {
    nlohmann::json j;
    std::string game_uid_; of_godot_variant(game_uid, &game_uid_);
    std::vector<std::string> player_move_; of_godot_variant(player_move, &player_move_);
    adrestia_networking::create_submit_move_call(j, game_uid_, player_move_);
    return String(j.dump().c_str());
  }

  IMPL_UNIT(create_get_stats_call);
  IMPL_UNIT(create_deactivate_account_call);
  IMPL_STRING(create_get_user_profile_call);
  IMPL_STRING(create_follow_user_call);
  IMPL_STRING(create_unfollow_user_call);
  IMPL_UNIT(create_get_friends_call);
}
