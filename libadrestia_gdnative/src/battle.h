#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <battle.h>

#include "macros.h"

namespace godot {
  class Battle : public godot::GodotScript<Reference>, public OwnerOrPointer<::Battle> {
    GODOT_CLASS(Battle)
   public:
    static void _register_methods();

    String get_id() const;
    INTF_NULLABLE
    INTF_TO_JSONABLE
  };
}
