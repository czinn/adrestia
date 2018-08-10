#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <game_rules.h>

namespace godot {
  class GameRules : public godot::GodotScript<Reference> {
    GODOT_CLASS(GameRules)
   public:
    static void _register_methods();
    ::GameRules _game_rules;

    String get_id() const;
    Variant as_json();
  };
}
