#include "player_view.h"
#include "macros.h"

using namespace godot;

namespace godot {
  void PlayerView::_register_methods() {
    register_method("as_json", &PlayerView::as_json);
  }

  Variant PlayerView::as_json() {
    return to_godot_json(_player_view);
  }
}
