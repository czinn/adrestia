#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <unit.h>
#include <unit_kind.h>

#include "macros.h"

namespace godot {
  class Unit : public godot::GodotScript<Reference> {
    GODOT_CLASS(Unit)
   public:
    Unit();
    ~Unit();
    static void _register_methods();
    ::Unit _unit;

    String get_id() const;
    INTF_TO_JSONABLE
  };
}
