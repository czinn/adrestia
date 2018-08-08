#include "game_state.h"
#include "macros.h"

using namespace godot;

namespace godot {
  void GameState::_register_methods() {
    register_method("init", &GameState::init);
    register_method("perform_action", &GameState::perform_action);
    register_method("as_json", &GameState::as_json);
  }

  void GameState::init(GameRules *rules, int num_players) {
    _game_state = std::make_shared<::GameState>(rules->_game_rules, num_players);
  }

  bool GameState::perform_action(int pid, Action *action) {
    return _game_state->perform_action(pid, *action->_action);
  }

  Variant GameState::as_json() const {
    return to_godot_json(*_game_state);
  }
}
