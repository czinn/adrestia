#include "unit.h"

using namespace godot;

#define CLASSNAME Unit

namespace godot {
  void Unit::_register_methods() {
    REGISTER_TO_JSONABLE
  }

  IMPL_TO_JSONABLE
}
