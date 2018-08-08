#include "player.h"

using namespace godot;

namespace godot {
  void Player::_register_methods() {
    REGISTER_TO_JSONABLE(Player)
  }

  IMPL_TO_JSONABLE(Player, _player)
}
