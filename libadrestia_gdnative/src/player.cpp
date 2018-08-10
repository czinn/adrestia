#include "player.h"
#include "macros.h"

using namespace godot;

namespace godot {
  void Player::_register_methods() {
    register_method("as_json", &Player::as_json);
  }

  Variant Player::as_json() {
    return to_godot_json(_player);
  }
}
