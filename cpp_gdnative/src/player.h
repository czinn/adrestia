#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <player.h>

#include "tech.h"
#include "unit.h"
#include "macros.h"

namespace godot {
  class Player : public godot::GodotScript<Reference>, public OwnerOrPointer<::Player> {
    GODOT_CLASS(Player)
   private:
    godot::Ref<godot::NativeScript> Tech_;
    godot::Ref<godot::NativeScript> Unit_;
   public:
    Player();
    static const char *resource_path;
    static void _register_methods();

    INTF_SETGET(Variant, units);
    INTF_SETGET(bool, alive);
    INTF_SETGET(int, coins);
    INTF_SETGET(Variant, tech);
    INTF_SETGET(int, next_unit);

    INTF_NULLABLE;
    INTF_TO_JSONABLE;
  };
}
