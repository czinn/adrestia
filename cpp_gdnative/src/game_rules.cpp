#include "game_rules.h"

using namespace godot;

#define CLASSNAME GameRules

namespace godot {
  const char *GameRules::resource_path = "res://native/game_rules.gdns";

  GameRules::GameRules() {
    UnitKind_ = ResourceLoader::load(UnitKind::resource_path);
  }

  void GameRules::_register_methods() {
    register_method("get_unit_cap", &GameRules::get_unit_cap);
    register_method("get_unit_kinds", &GameRules::get_unit_kinds);
    register_method("get_unit_kind", &GameRules::get_unit_kind);
    REGISTER_NULLABLE
    REGISTER_JSONABLE
  }

  FORWARD_GETTER(int, get_unit_cap)

  Dictionary GameRules::get_unit_kinds() {
    Dictionary result;
    for (auto &[kind_id, kind] : _ptr->get_unit_kinds()) {
      auto [v, uk] = instance<UnitKind>(UnitKind_);
      uk->set_ptr(const_cast<::UnitKind*>(&kind), owner);
      result[String(kind_id.c_str())] = v;
    }
    return result;
  }

  Variant GameRules::get_unit_kind(String id) {
    auto [v, kind] = instance<UnitKind>(UnitKind_);
    auto &kinds = const_cast<std::map<std::string, ::UnitKind>&>(_ptr->get_unit_kinds());
    const std::string key(id.ascii().get_data());
    kind->set_ptr(&kinds[key], owner);
    return v;
  }

  IMPL_NULLABLE
  IMPL_JSONABLE
}