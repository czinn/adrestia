#include "colour.h"

using namespace godot;

#define CLASSNAME Colour

namespace godot {
  void Colour::_register_methods() {
    REGISTER_JSONABLE
  }

  IMPL_JSONABLE
}
