#include "unit_kind.h"

#define CLASSNAME UnitKind

using namespace godot;

namespace godot {
  const char *UnitKind::resource_path = "res://native/unit_kind.gdns";

  UnitKind::UnitKind() {
    Colour_ = ResourceLoader::load(Colour::resource_path);
    Tech_ = ResourceLoader::load(Tech::resource_path);
  }

  void UnitKind::_register_methods() {
    REGISTER_METHOD(get_id)
    REGISTER_METHOD(get_name)
    REGISTER_METHOD(get_colour)
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
    REGISTER_METHOD(get_label)
    REGISTER_NULLABLE
    REGISTER_JSONABLE
  }

  FORWARD_STRING_GETTER(get_id)
  FORWARD_STRING_GETTER(get_name)
  Variant CLASSNAME::get_colour() const {
    auto [v, kind] = instance<Colour>(Colour_);
    kind->set(_ptr->get_colour());
    return v;
  }
  FORWARD_GETTER(int, get_health)
  FORWARD_GETTER(int, get_shields)
  FORWARD_GETTER(int, get_width)
  FORWARD_GETTER(int, get_build_time)
  FORWARD_ARRAY_GETTER(get_attack)
  FORWARD_GETTER(int, get_cost)
  FORWARD_SMART_PTR_GETTER(Tech, get_tech)
  FORWARD_GETTER(int, get_font)
  FORWARD_STRING_GETTER(get_image)
  FORWARD_ARRAY_GETTER(get_tiles)
  FORWARD_GETTER(String, get_label)

  IMPL_NULLABLE
  IMPL_JSONABLE
}
