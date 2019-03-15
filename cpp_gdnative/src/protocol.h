#pragma once
#include <Godot.hpp>

#include "macros.h"

namespace godot {
  class Protocol : public godot::GodotScript<Reference> {
      GODOT_CLASS(Protocol)
    public:
      static void _register_methods();
      Protocol();
      ~Protocol();

      godot::String create_floop_call();
      godot::String create_establish_connection_call(godot::String version);
      godot::String create_register_new_account_call(godot::String password, bool debug, godot::String user_name, godot::String platform);
      godot::String create_authenticate_call(godot::String uuid, godot::String password);
      godot::String create_abort_game_call(godot::String game_uid);
      godot::String create_change_user_name_call(godot::String user_name);
      godot::String create_matchmake_me_call(godot::Variant rules, godot::Variant selected_books, godot::String target_friend_code);
      godot::String create_submit_move_call(godot::String game_uid, godot::Variant player_move);
      godot::String create_get_stats_call();
      godot::String create_deactivate_account_call();
      godot::String create_get_user_profile_call(godot::String friend_code);
      godot::String create_follow_user_call(godot::String friend_code);
      godot::String create_unfollow_user_call(godot::String friend_code);
      godot::String create_get_friends_call();
  };
}
