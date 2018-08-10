#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <action.h>

namespace godot {
  class Action : public godot::GodotScript<Reference> {
    GODOT_CLASS(Action)
   public:
    static void _register_methods();
    ::Action _action;

    String get_id() const;
    Variant as_json();
  };
}
