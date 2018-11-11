#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <battle.h>

#include "player_view.h"

#include "macros.h"

namespace godot {
  class Attack : public godot::GodotScript<Reference>, public OwnerOrPointer<::Attack> {
    GODOT_CLASS(Attack)
   public:
    static const char *resource_path;
    static void _register_methods();

    INTF_SETGET(int, from_player);
    INTF_SETGET(int, from_unit);
    INTF_SETGET(int, to_player);
    INTF_SETGET(int, to_unit);
    INTF_SETGET(int, damage);

    INTF_NULLABLE
    INTF_TO_JSONABLE
  };

  class Battle : public godot::GodotScript<Reference>, public OwnerOrPointer<::Battle> {
    GODOT_CLASS(Battle)
   private:
    godot::Ref<godot::NativeScript> Attack_;
    godot::Ref<godot::NativeScript> PlayerView_;

   public:
    Battle();
    static const char *resource_path;
    static void _register_methods();

    Array get_players() const;
    Array get_players_after() const;
    Array get_attacks() const;

    INTF_NULLABLE
    INTF_TO_JSONABLE
  };
}
