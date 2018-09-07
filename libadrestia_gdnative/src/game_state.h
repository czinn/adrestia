#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <game_state.h>

#include "action.h"
#include "battle.h"
#include "player.h"
#include "game_rules.h"

#include "macros.h"

namespace godot {
  class GameState : public godot::GodotScript<Reference>, public OwnerOrPointer<::GameState> {
    GODOT_CLASS(GameState)
   private:
    godot::Ref<godot::NativeScript> Action_;
    godot::Ref<godot::NativeScript> Battle_;
    godot::Ref<godot::NativeScript> GameRules_;
    godot::Ref<godot::NativeScript> Player_;
   public:
    GameState();
    static const char *resource_path;
    static void _register_methods();

    void init(GameRules *rules, int num_players);
    bool perform_action(int pid, Action *action);
    Array get_winners() const;
    Variant get_rules() const;
    int get_turn() const;
    Array get_players() const;
    Array get_action_log() const;
    Array get_battles() const;

    INTF_NULLABLE
    INTF_TO_JSONABLE
  };
}
