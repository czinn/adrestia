#include "unit.h"
#include "macros.h"

#include <unit_kind.h>

using namespace godot;

namespace godot {
  // TODO(jim) jim: Terrible hack because UnitKind has no default constructor.
  // In general, all scripts need a default constructor due to the way
  // godot-cpp implements _godot_class_instance_func.
  Unit::Unit() : _unit(UnitKind()) { }

  Unit::~Unit() { }

  void Unit::_register_methods() {
    register_method("as_json", &Unit::as_json);
  }

  Variant Unit::as_json() {
    return to_godot_json(_unit);
  }
}
