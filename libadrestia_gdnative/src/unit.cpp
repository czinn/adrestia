#include "unit.h"

using namespace godot;

namespace godot {
  // TODO(jim) jim: Terrible hack because UnitKind has no default constructor.
  // In general, all scripts need a default constructor due to the way
  // godot-cpp implements _godot_class_instance_func.
  Unit::Unit() : _unit(UnitKind()) { }

  Unit::~Unit() { }

  void Unit::_register_methods() {
    REGISTER_TO_JSONABLE(Unit)
  }

  IMPL_TO_JSONABLE(Unit, _unit)
}
