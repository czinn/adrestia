#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <unit_kind.h>

#include "colour.h"
#include "tech.h"
#include "macros.h"

namespace godot {
  class UnitKind : public godot::GodotScript<Reference>, public OwnerOrPointer<::UnitKind> {
    GODOT_CLASS(UnitKind)
   private:
    godot::Ref<godot::NativeScript> Colour_;
    godot::Ref<godot::NativeScript> Tech_;

   public:
    UnitKind();
    static const char *resource_path;
    static void _register_methods();

    String get_id() const;
    String get_name() const;
    Variant get_colour() const;
    int get_health() const;
    int get_shields() const;
    int get_width() const;
    int get_build_time() const;
    Array get_attack() const;
    int get_cost() const;
    Variant get_tech() const;
    int get_font() const;
    String get_image() const;
    Array get_tiles() const;
    String get_label() const;

    INTF_NULLABLE
    INTF_JSONABLE
  };
}
