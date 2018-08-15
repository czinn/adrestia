#include "game_state.h"
#include "macros.h"

using namespace godot;

#define CLASSNAME GameState

namespace godot {
  void GameState::_register_methods() {
    register_method("init", &GameState::init);
    register_method("perform_action", &GameState::perform_action);
    REGISTER_TO_JSONABLE
  }

  void GameState::init(GameRules *rules, int num_players) {
    set_ptr(new ::GameState(*rules->_ptr, num_players));
  }

  bool GameState::perform_action(int pid, Action *action) {
    return _ptr->perform_action(pid, *action->_ptr);
  }

  Array GameState::players() const {
    // TODO(jim): implement
    return Array();
  }

  IMPL_TO_JSONABLE
}
