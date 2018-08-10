#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <player.h>

namespace godot {
  class Player : public godot::GodotScript<Reference> {
    GODOT_CLASS(Player)
   public:
    static void _register_methods();
    ::Player _player;

    String get_id() const;
    Variant as_json();
  };
}
