#include "unit_kind.h"
#include "macros.h"

using namespace godot;

namespace godot {
  void UnitKind::_register_methods() {
    register_method("get_id", &UnitKind::get_id);
    register_method("as_json", &UnitKind::as_json);
  }

  String UnitKind::get_id() const {
    return String(_unit_kind.get_id().c_str());
  }

  Variant UnitKind::as_json() {
    return to_godot_json(_unit_kind);
  }
}
