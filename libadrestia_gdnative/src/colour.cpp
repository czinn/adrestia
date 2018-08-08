#include "colour.h"

using namespace godot;

namespace godot {
  void Colour::_register_methods() {
    REGISTER_JSONABLE(Colour)
  }

  IMPL_JSONABLE(Colour, _colour)
}
