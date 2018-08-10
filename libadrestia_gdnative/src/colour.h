#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <colour.h>

namespace godot {
  class Colour : public godot::GodotScript<Reference> {
    GODOT_CLASS(Colour)
   public:
    static void _register_methods();
    ::Colour _colour;
    Variant as_json();
  };
}
