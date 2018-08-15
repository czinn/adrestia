#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <tech.h>
#include "colour.h"
// TODO jim: <tech.h> is in units_cpp. "colour.h" is in this folder. serves as
// an adapter. I think this is a brilliant hack, but it may lead to confusion
// so maybe we should change it.

namespace godot {
  class Tech : public godot::GodotScript<Reference>, public OwnerOrPointer<::Tech> {
    GODOT_CLASS(Tech)
   public:
    static const char *resource_path;
    static void _register_methods();
    // jim: This works because of some incredible C++ templating magic (ArgCast
    // in Godot.hpp).

    bool good() const;
    void init(int r, int g, int b);
    void increment(Colour *c);
    void add(Tech *other);
    bool includes(Tech *other);

    INTF_JSONABLE
  };
}
