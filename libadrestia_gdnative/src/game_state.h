#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <game_state.h>

#include "action.h"
#include "game_rules.h"

namespace godot {
  class GameState : public godot::GodotScript<Reference>, public OwnerOrPointer<::GameState> {
    GODOT_CLASS(GameState)
   public:
    static void _register_methods();

    void init(GameRules *rules, int num_players);
    bool perform_action(int pid, Action *action);
    String get_id() const;
    Array players() const;
    INTF_TO_JSONABLE
  };
}
