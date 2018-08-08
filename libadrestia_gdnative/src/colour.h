#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <colour.h>

#include "macros.h"

namespace godot {
  class Colour : public godot::GodotScript<Reference> {
    GODOT_CLASS(Colour)
   public:
    ::Colour _colour;

    static void _register_methods();

    INTF_JSONABLE
  };
}
