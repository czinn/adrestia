#include "game_view.h"

using namespace godot;

#define CLASSNAME GameView

namespace godot {
  const char *GameView::resource_path = "res://native/game_view.gdns";

  void GameView::_register_methods() {
    REGISTER_NULLABLE
    REGISTER_TO_JSONABLE
  }

  IMPL_NULLABLE
  IMPL_TO_JSONABLE
}
