#include "game_state.h"
#include "macros.h"
#include <game_rules.h>

using namespace godot;

namespace godot {
  GameState::GameState() : _game_state(GameRules(), 2) {}
  GameState::~GameState() {}
  void GameState::_register_methods() {
    register_method("as_json", &GameState::as_json);
  }

  Variant GameState::as_json() {
    return to_godot_json(_game_state);
  }
}
