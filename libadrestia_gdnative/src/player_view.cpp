#include "player_view.h"

using namespace godot;

#define CLASSNAME PlayerView

namespace godot {
  const char *PlayerView::resource_path = "res://native/player_view.gdns";

  void PlayerView::_register_methods() {
    REGISTER_NULLABLE
    REGISTER_TO_JSONABLE
  }

  IMPL_NULLABLE
  IMPL_TO_JSONABLE
}
