#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <battle.h>

#include "macros.h"

namespace godot {
  class Battle : public godot::GodotScript<Reference> {
    GODOT_CLASS(Battle)
   public:
    static void _register_methods();
    ::Battle _battle;

    String get_id() const;
    INTF_TO_JSONABLE
  };
}
