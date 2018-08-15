#include "unit.h"

using namespace godot;

namespace godot {
  void Unit::_register_methods() {
    REGISTER_TO_JSONABLE(Unit)
  }

  IMPL_TO_JSONABLE(Unit, *_ptr)
}
