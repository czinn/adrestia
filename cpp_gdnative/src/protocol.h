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
      godot::String create_establish_connection_call();
      godot::String create_register_new_account_call(godot::String password);
      godot::String create_authenticate_call(godot::String uuid, godot::String password);
      godot::String create_change_user_name_call(godot::String user_name);
      godot::String create_matchmake_me_call(godot::Variant selected_books);
  };
}
