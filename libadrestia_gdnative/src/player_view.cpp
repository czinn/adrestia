#include "player_view.h"

#define CLASSNAME PlayerView

using namespace godot;

namespace godot {
  void PlayerView::_register_methods() {
    REGISTER_TO_JSONABLE
  }

  IMPL_TO_JSONABLE(*_ptr)
}
