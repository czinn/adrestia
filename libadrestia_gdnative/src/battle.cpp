#include "battle.h"

using namespace godot;

#define CLASSNAME Battle

namespace godot {
  void Battle::_register_methods() {
    REGISTER_TO_JSONABLE
  }

  IMPL_TO_JSONABLE
}
