#include "action.h"
#include "macros.h"

using namespace godot;

namespace godot {
  void Action::_register_methods() {
    register_method("as_json", &Action::as_json);
  }

  Variant Action::as_json() {
    return to_godot_json(_action);
  }
}
