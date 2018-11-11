#include "colour.h"

using namespace godot;

#define CLASSNAME Colour

namespace godot {
  const char *Colour::resource_path = "res://native/colour.gdns";

  void Colour::_register_methods() {
    REGISTER_METHOD(to_color)
    REGISTER_METHOD(set_RED)
    REGISTER_METHOD(set_GREEN)
    REGISTER_METHOD(set_BLUE)
    REGISTER_METHOD(set_char)
    REGISTER_NULLABLE
    REGISTER_JSONABLE
  }

  void Colour::set(::Colour colour) {
    set_ptr(new ::Colour(colour));
  }

  Variant Colour::to_color() {
    if (!_ptr) return Color(1, 0, 0);
    switch (*_ptr) {
      case BLACK: return Color(0, 0, 0);
      case RED  : return Color(0.8, 0.25, 0.25);
      case GREEN: return Color(0.2, 0.66, 0.35);
      case BLUE : return Color(0.2, 0.6, 0.85);
      default: return Color(1, 0, 0);
    }
  }

  void Colour::set_RED() {
    set(::RED);
  }

  void Colour::set_GREEN() {
    set(::GREEN);
  }

  void Colour::set_BLUE() {
    set(::BLUE);
  }

  void Colour::set_char(String c) {
    switch (c[0]) {
      case 'r': set_RED();
      case 'g': set_GREEN();
      case 'b': set_BLUE();
    }
  }

  IMPL_NULLABLE
  IMPL_JSONABLE
}
