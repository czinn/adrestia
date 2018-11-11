#include "unit.h"

using namespace godot;

#define CLASSNAME Unit

namespace godot {
  const char *Unit::resource_path = "res://native/unit.gdns";

  Unit::Unit() {
    UnitKind_ = ResourceLoader::load(UnitKind::resource_path);
  }

  void Unit::_register_methods() {
    REGISTER_SETGET(kind, Variant())
    REGISTER_SETGET(health, -1)
    REGISTER_SETGET(shields, -1)
    REGISTER_SETGET(build_time, -1)
    REGISTER_NULLABLE
    REGISTER_TO_JSONABLE
  }

  IMPL_SETGET_CONST_REF(UnitKind, kind);
  IMPL_SETGET(int, health);
  IMPL_SETGET(int, shields);
  IMPL_SETGET(int, build_time);

  IMPL_NULLABLE;
  IMPL_TO_JSONABLE;
}
