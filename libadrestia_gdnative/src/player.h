#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <player.h>

#include "macros.h"

namespace godot {
  class Player : public godot::GodotScript<Reference>, public OwnerOrPointer<::Player> {
    GODOT_CLASS(Player)
   public:
    static const char *resource_path;
    static void _register_methods();

    INTF_NULLABLE
    INTF_TO_JSONABLE
  };
}
