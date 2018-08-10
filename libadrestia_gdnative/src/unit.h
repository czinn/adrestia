#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <unit.h>

namespace godot {
  class Unit : public godot::GodotScript<Reference> {
    GODOT_CLASS(Unit)
   public:
    Unit();
    ~Unit();
    static void _register_methods();
    ::Unit _unit;

    String get_id() const;
    Variant as_json();
  };
}
