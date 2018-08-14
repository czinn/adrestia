#include "colour.h"

#define CLASSNAME Colour

using namespace godot;

namespace godot {
  void Colour::_register_methods() {
    REGISTER_JSONABLE
  }

  IMPL_JSONABLE(*_ptr)
}
