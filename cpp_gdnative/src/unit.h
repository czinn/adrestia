#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <unit.h>
#include <unit_kind.h>

#include "unit_kind.h"
#include "macros.h"

namespace godot {
  class Unit : public godot::GodotScript<Reference>, public OwnerOrPointer<::Unit> {
    GODOT_CLASS(Unit)
   private:
    godot::Ref<godot::NativeScript> UnitKind_;
   public:
    static const char *resource_path;
    Unit();
    static void _register_methods();

    INTF_SETGET(Variant, kind);
    INTF_SETGET(int, health);
    INTF_SETGET(int, shields);
    INTF_SETGET(int, build_time);

    INTF_NULLABLE;
    INTF_TO_JSONABLE;
  };
}
