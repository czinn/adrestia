#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <unit_kind.h>

#include "macros.h"

namespace godot {
  class UnitKind : public godot::GodotScript<Reference> {
    GODOT_CLASS(UnitKind)
   public:
    static void _register_methods();
    ::UnitKind _unit_kind;

    String get_id() const;

    INTF_JSONABLE
  };
}
