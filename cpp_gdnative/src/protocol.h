#pragma once
#include <Godot.hpp>

namespace godot {
  class Protocol : public godot::GodotScript<Reference> {
      GODOT_CLASS(Protocol)
    public:
      static void _register_methods();
      Protocol();
      ~Protocol();

      godot::String make_floop_request();
  };
}
