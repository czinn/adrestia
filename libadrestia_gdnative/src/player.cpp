#include "player.h"

using namespace godot;

#define CLASSNAME Player

namespace godot {
  const char *Player::resource_path = "res://native/player.gdns";

  Player::Player() {
    Tech_ = ResourceLoader::load(Tech::resource_path);
    Unit_ = ResourceLoader::load(Unit::resource_path);
  }

  void Player::_register_methods() {
    REGISTER_SETGET(units, Variant());
    REGISTER_SETGET(alive, true);
    REGISTER_SETGET(coins, -1);
    REGISTER_SETGET(tech, Variant());
    REGISTER_SETGET(next_unit, -1);
    REGISTER_NULLABLE;
    REGISTER_TO_JSONABLE;
  }

  IMPL_SETGET_REF_DICT(Unit, units);
  IMPL_SETGET(bool, alive);
  IMPL_SETGET(int, coins);
  IMPL_SETGET_REF(Tech, tech);
  IMPL_SETGET(int, next_unit);
  IMPL_NULLABLE;
  IMPL_TO_JSONABLE;
}
