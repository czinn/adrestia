#include "player.h"

using namespace godot;

#define CLASSNAME Player

namespace godot {
  void Player::_register_methods() {
    REGISTER_TO_JSONABLE
  }

  IMPL_TO_JSONABLE
}
