#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <NativeScript.hpp>
#include <ResourceLoader.hpp>
#include <game_rules.h>
#include "unit_kind.h"
#include "macros.h"

namespace godot {
  class GameRules : public godot::GodotScript<Reference>, public OwnerOrPointer<::GameRules> {
    GODOT_CLASS(GameRules)
   private:
    godot::Ref<godot::NativeScript> UnitKind_;
   public:
    GameRules();
    static const char *resource_path;
    static void _register_methods();

    int get_unit_cap() const;
    godot::Dictionary get_unit_kinds();
    godot::Variant get_unit_kind(godot::String id);

    INTF_NULLABLE
    INTF_JSONABLE
  };
}
