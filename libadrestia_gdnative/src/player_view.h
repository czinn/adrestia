#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <player_view.h>

#include "macros.h"

namespace godot {
  class PlayerView : public godot::GodotScript<Reference> {
    GODOT_CLASS(PlayerView)
   public:
    static void _register_methods();
    ::PlayerView _player_view;

    String get_id() const;
    INTF_TO_JSONABLE
  };
}
