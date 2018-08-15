#include "unit_kind.h"

#define CLASSNAME UnitKind

using namespace godot;

namespace godot {
  const char *UnitKind::resource_path = "res://native/unit_kind.gdns";

  UnitKind::UnitKind() {
    Tech_ = ResourceLoader::load(Tech::resource_path);
  }

  void UnitKind::_register_methods() {
    register_method("get_id", &UnitKind::get_id);
    register_method("get_name", &UnitKind::get_name);
    register_method("get_image", &UnitKind::get_image);
    register_method("get_health", &UnitKind::get_health);
    register_method("get_shields", &UnitKind::get_shields);
    register_method("get_width", &UnitKind::get_width);
    register_method("get_build_time", &UnitKind::get_build_time);
    register_method("get_attack", &UnitKind::get_attack);
    register_method("get_cost", &UnitKind::get_cost);
    register_method("get_tech", &UnitKind::get_tech);
    register_method("get_font", &UnitKind::get_font);
    register_method("get_image", &UnitKind::get_image);
    register_method("get_tiles", &UnitKind::get_tiles);
    REGISTER_NULLABLE
    REGISTER_JSONABLE
  }

  FORWARD_STRING_GETTER(get_id)
  FORWARD_STRING_GETTER(get_name)
  FORWARD_GETTER(int, get_health)
  FORWARD_GETTER(int, get_shields)
  FORWARD_GETTER(int, get_width)
  FORWARD_GETTER(int, get_build_time)
  FORWARD_ARRAY_GETTER(get_attack)
  FORWARD_GETTER(int, get_cost)
  FORWARD_SMART_PTR(Tech, get_tech)
  FORWARD_GETTER(int, get_font)
  FORWARD_STRING_GETTER(get_image)
  FORWARD_ARRAY_GETTER(get_tiles)

  IMPL_NULLABLE
  IMPL_JSONABLE
}
