#include "game_rules.h"

using namespace godot;

namespace godot {
  GameRules::GameRules() {
    UnitKind_ = ResourceLoader::load(UnitKind::resource_path);
  }

  void GameRules::_register_methods() {
    register_method("get_unit_cap", &GameRules::get_unit_cap);
    register_method("get_unit_kinds", &GameRules::get_unit_kinds);
    register_method("get_unit_kind", &GameRules::get_unit_kind);
    REGISTER_JSONABLE(GameRules)
  }

  int GameRules::get_unit_cap() const {
    return _ptr->get_unit_cap();
  }

  Dictionary GameRules::get_unit_kinds() {
    Dictionary d;
    for (auto &[kind_id, kind] : _ptr->get_unit_kinds()) {
      auto [v, uk] = instance<UnitKind>(UnitKind_);
      uk->set_ptr(const_cast<::UnitKind*>(&kind), owner);
      d[String(kind_id.c_str())] = v;
    }
    return d;
  }

  Variant GameRules::get_unit_kind(String id) {
    auto [v, kind] = instance<UnitKind>(UnitKind_);
    auto &kinds = const_cast<std::map<std::string, ::UnitKind>&>(_ptr->get_unit_kinds());
    const std::string key(id.ascii().get_data());
    kind->set_ptr(&kinds[key], owner);
    return v;
  }

  IMPL_JSONABLE(GameRules, *_ptr)
}
