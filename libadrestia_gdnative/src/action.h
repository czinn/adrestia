#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <action.h>

#include "macros.h"
#include "colour.h"

namespace godot {
  class Action : public godot::GodotScript<Reference> {
    GODOT_CLASS(Action)
   public:
    static void _register_methods();
    std::shared_ptr<::Action> _action;

    void init_tech_colour(Colour *colour);
    void init_units(Variant units);
    String get_id() const;

    INTF_JSONABLE
  };
}
