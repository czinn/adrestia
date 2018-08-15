#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <player_view.h>

#include "macros.h"

namespace godot {
  class PlayerView : public godot::GodotScript<Reference>, public OwnerOrPointer<::PlayerView> {
    GODOT_CLASS(PlayerView)
   public:
    static void _register_methods();

    INTF_NULLABLE
    INTF_TO_JSONABLE
  };
}
