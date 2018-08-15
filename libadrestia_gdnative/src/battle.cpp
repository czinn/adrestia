#include "battle.h"

using namespace godot;

namespace godot {
  void Battle::_register_methods() {
    REGISTER_TO_JSONABLE(Battle)
  }

  IMPL_TO_JSONABLE(Battle, *_ptr)
}
