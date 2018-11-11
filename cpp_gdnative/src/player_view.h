#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <player_view.h>

#include "unit.h"
#include "macros.h"

namespace godot {
  class PlayerView : public godot::GodotScript<Reference>, public OwnerOrPointer<::PlayerView> {
    GODOT_CLASS(PlayerView)
   private:
    godot::Ref<godot::NativeScript> Unit_;
   public:
    PlayerView();
    static const char *resource_path;
    static void _register_methods();

    INTF_SETGET(Variant, units);
    INTF_SETGET(bool, alive);
    INTF_SETGET(int, coins);
    INTF_NULLABLE;
    INTF_TO_JSONABLE;
  };
}
