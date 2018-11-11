#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <action.h>

#include "macros.h"
#include "colour.h"

namespace godot {
  class Action : public godot::GodotScript<Reference>, public OwnerOrPointer<::Action> {
    GODOT_CLASS(Action)
   public:
    static const char *resource_path;
    static void _register_methods();

    void init_tech_colour(Colour *colour);
    void init_units(Array units);

    INTF_NULLABLE
    INTF_JSONABLE
  };
}
