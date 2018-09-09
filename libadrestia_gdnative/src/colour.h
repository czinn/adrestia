#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <Color.hpp>
#include <colour.h>

#include "macros.h"

namespace godot {
  class Colour : public godot::GodotScript<Reference>, public OwnerOrPointer<::Colour> {
    GODOT_CLASS(Colour)
   public:
    static const char *resource_path;
    static void _register_methods();

    void set(::Colour colour);
    Variant to_color();

    INTF_NULLABLE
    INTF_JSONABLE

    // TODO: jim: Is there a better way to forward enums?
    void set_RED();
    void set_GREEN();
    void set_BLUE();

    void set_char(String c);
  };
}
