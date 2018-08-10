#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <game_rules.h>
#include "macros.h"

namespace godot {
  class GameRules : public godot::GodotScript<Reference>, public OwnerOrPointer<::GameRules> {
    GODOT_CLASS(GameRules)
   public:
    static void _register_methods();

    INTF_JSONABLE
  };
}
