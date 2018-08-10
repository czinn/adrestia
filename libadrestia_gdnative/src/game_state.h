#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <game_state.h>

namespace godot {
  class GameState : public godot::GodotScript<Reference> {
    GODOT_CLASS(GameState)
   public:
    GameState();
    ~GameState();

    static void _register_methods();
    ::GameState _game_state;

    String get_id() const;
    Variant as_json();
  };
}

