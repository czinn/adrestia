#include "player.h"

#define CLASSNAME Player

using namespace godot;

namespace godot {
  void Player::_register_methods() {
    REGISTER_TO_JSONABLE
  }

  IMPL_TO_JSONABLE(*_ptr)
}
