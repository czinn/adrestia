#include "colour.h"

using namespace godot;

#define CLASSNAME Colour

namespace godot {
  void Colour::_register_methods() {
    REGISTER_NULLABLE
    REGISTER_JSONABLE
  }

  IMPL_NULLABLE
  IMPL_JSONABLE
}
