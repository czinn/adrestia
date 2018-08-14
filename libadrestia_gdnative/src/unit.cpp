#include "unit.h"

#define CLASSNAME Unit

using namespace godot;

namespace godot {
  void Unit::_register_methods() {
    REGISTER_TO_JSONABLE
  }

  IMPL_TO_JSONABLE(*_ptr)
}
