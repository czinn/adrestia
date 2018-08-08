#include "player_view.h"

using namespace godot;

namespace godot {
  void PlayerView::_register_methods() {
    REGISTER_TO_JSONABLE(PlayerView)
  }

  IMPL_TO_JSONABLE(PlayerView, _player_view)
}
