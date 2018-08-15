#include "unit_kind.h"

#define CLASSNAME UnitKind

using namespace godot;

namespace godot {
  const char *UnitKind::resource_path = "res://native/unit_kind.gdns";

  UnitKind::UnitKind() {
    Tech_ = ResourceLoader::load(Tech::resource_path);
  }

  void UnitKind::_register_methods() {
    REGISTER_METHOD(get_id)
    REGISTER_METHOD(get_name)
    REGISTER_METHOD(get_image)
    REGISTER_METHOD(get_health)
    REGISTER_METHOD(get_shields)
    REGISTER_METHOD(get_width)
    REGISTER_METHOD(get_build_time)
    REGISTER_METHOD(get_attack)
    REGISTER_METHOD(get_cost)
    REGISTER_METHOD(get_tech)
    REGISTER_METHOD(get_font)
    REGISTER_METHOD(get_image)
    REGISTER_METHOD(get_tiles)
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
