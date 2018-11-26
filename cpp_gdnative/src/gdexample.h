#pragma once
#include <Godot.hpp>
#include <Sprite.hpp>

// Example gdscript that can be attached to a sprite, and that moves the sprite
// in a wavy manner.

namespace godot {
  class gdexample : public godot::GodotScript<Sprite> {
    GODOT_CLASS(gdexample)
   private:
    float time_passed;
   public:
    static void _register_methods();
    gdexample();
    ~gdexample();
    void _process(float delta);
  };
}