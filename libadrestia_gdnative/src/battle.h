#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <battle.h>

namespace godot {
  class Battle : public godot::GodotScript<Reference> {
    GODOT_CLASS(Battle)
   public:
    static void _register_methods();
    ::Battle _battle;

    String get_id() const;
    Variant as_json();
  };
}
