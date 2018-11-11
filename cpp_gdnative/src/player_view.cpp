#include "player_view.h"

using namespace godot;

#define CLASSNAME PlayerView

namespace godot {
  const char *PlayerView::resource_path = "res://native/player_view.gdns";

  PlayerView::PlayerView() {
    Unit_ = ResourceLoader::load(Unit::resource_path);
  }

  void PlayerView::_register_methods() {
    REGISTER_SETGET(units, Variant());
    REGISTER_SETGET(alive, true);
    REGISTER_SETGET(coins, -1);
    REGISTER_NULLABLE
    REGISTER_TO_JSONABLE
  }

  IMPL_SETGET_REF_DICT(Unit, units);
  IMPL_SETGET(bool, alive);
  IMPL_SETGET(int, coins);
  IMPL_NULLABLE;
  IMPL_TO_JSONABLE;
}
