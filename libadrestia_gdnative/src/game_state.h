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
    static const char *resource_path;
    static void _register_methods();

    void init(GameRules *rules, int num_players);
    bool perform_action(int pid, Action *action);
    // TODO jim: GameView
    String get_id() const;
    Array players() const;

    INTF_NULLABLE
    INTF_TO_JSONABLE
  };
}
