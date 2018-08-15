#include "player.h"

using namespace godot;

#define CLASSNAME Player

namespace godot {
  void Player::_register_methods() {
    REGISTER_NULLABLE
    REGISTER_TO_JSONABLE
  }

  IMPL_NULLABLE
  IMPL_TO_JSONABLE
}
