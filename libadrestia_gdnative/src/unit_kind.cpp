#include "unit_kind.h"

#define CLASSNAME UnitKind

using namespace godot;

namespace godot {
  const char *UnitKind::resource_path = "res://native/unit_kind.gdns";

  UnitKind::UnitKind() {
    Tech_ = ResourceLoader::load(godot::Tech::resource_path);
  }

  void UnitKind::_register_methods() {
    register_method("get_id", &UnitKind::get_id);
    REGISTER_JSONABLE
  }

  FORWARD_STRING_GETTER(get_id);
  FORWARD_STRING_GETTER(get_name);
  FORWARD_GETTER(int, get_health);
  FORWARD_GETTER(int, get_shields);
  FORWARD_GETTER(int, get_width);
  FORWARD_GETTER(int, get_build_time);
  FORWARD_ARRAY_GETTER(get_attack);
  FORWARD_GETTER(int, get_cost);
  FORWARD_SMART_PTR(Tech, get_tech);
  FORWARD_GETTER(int, get_font);
  FORWARD_STRING_GETTER(get_image);
  FORWARD_ARRAY_GETTER(get_tiles);
  String CLASSNAME::get_label() const {
    // Special: char -> String conversion
    return String(_ptr->get_label());
  }

  IMPL_JSONABLE(*_ptr)
}
