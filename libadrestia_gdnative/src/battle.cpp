#include "battle.h"
#include "macros.h"

using namespace godot;

namespace godot {
  void Battle::_register_methods() {
    register_method("as_json", &Battle::as_json);
  }

  Variant Battle::as_json() {
    return to_godot_json(_battle);
  }
}
