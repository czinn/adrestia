#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <game_state.h>

#include "game_rules.h"

namespace godot {
  class GameState : public godot::GodotScript<Reference> {
    GODOT_CLASS(GameState)
   public:
    static void _register_methods();
    std::shared_ptr<::GameState> _game_state;

    void init(GameRules *rules, int num_players);
    String get_id() const;
    Variant as_json();
  };
}

