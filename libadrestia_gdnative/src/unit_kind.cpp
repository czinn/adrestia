#include "unit_kind.h"

using namespace godot;

namespace godot {
  void UnitKind::_register_methods() {
    register_method("get_id", &UnitKind::get_id);
    REGISTER_JSONABLE(UnitKind)
  }

  String UnitKind::get_id() const {
    return String(_ptr->get_id().c_str());
  }

  IMPL_JSONABLE(UnitKind, *_ptr)
}
