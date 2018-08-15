#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <unit.h>
#include <unit_kind.h>

#include "macros.h"

namespace godot {
  class Unit : public godot::GodotScript<Reference>, OwnerOrPointer<::Unit> {
    GODOT_CLASS(Unit)
   public:
    static void _register_methods();

    String get_id() const;
    INTF_NULLABLE
    INTF_TO_JSONABLE
  };
}
