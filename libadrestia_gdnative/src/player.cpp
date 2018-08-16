#include "player.h"

using namespace godot;

#define CLASSNAME Player

namespace godot {
  const char *Player::resource_path = "res://native/player.gdns";

  void Player::_register_methods() {
    REGISTER_NULLABLE
    REGISTER_TO_JSONABLE
  }

  IMPL_NULLABLE
  IMPL_TO_JSONABLE
}
