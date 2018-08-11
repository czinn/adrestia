#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <unit_kind.h>

#include "macros.h"

namespace godot {
  class UnitKind : public godot::GodotScript<Reference>, public OwnerOrPointer<::UnitKind> {
    GODOT_CLASS(UnitKind)
   public:
    static void _register_methods();
    String get_id() const;
    static const char *resource_path;
    INTF_JSONABLE
  };
}
