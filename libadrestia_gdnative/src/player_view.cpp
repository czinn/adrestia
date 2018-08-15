#include "player_view.h"

using namespace godot;

#define CLASSNAME PlayerView

namespace godot {
  void PlayerView::_register_methods() {
    REGISTER_TO_JSONABLE
  }

  IMPL_TO_JSONABLE
}
