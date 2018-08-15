#include "unit.h"

using namespace godot;

#define CLASSNAME Unit

namespace godot {
  void Unit::_register_methods() {
    REGISTER_NULLABLE
    REGISTER_TO_JSONABLE
  }

  IMPL_NULLABLE
  IMPL_TO_JSONABLE
}
