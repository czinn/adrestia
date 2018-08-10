#include "colour.h"
#include "macros.h"

using namespace godot;

namespace godot {
  void Colour::_register_methods() {
    register_method("as_json", &godot::Colour::as_json);
  }

  Variant Colour::as_json() {
    return to_godot_json(_colour);
  }
}
