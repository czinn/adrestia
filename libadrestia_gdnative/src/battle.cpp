#include "battle.h"

#define CLASSNAME Battle

using namespace godot;

namespace godot {
  void Battle::_register_methods() {
    REGISTER_TO_JSONABLE
  }

  IMPL_TO_JSONABLE(*_ptr)
}
