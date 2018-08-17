#include <sstream>
#include "tech.h"
#include "macros.h"

using namespace godot;

#define CLASSNAME Tech

namespace godot {
  const char *Tech::resource_path = "res://native/tech.gdns";

  void Tech::_register_methods() {
    REGISTER_METHOD(init)
    REGISTER_METHOD(increment)
    REGISTER_METHOD(add)
    REGISTER_METHOD(includes)

    REGISTER_SETGET(red, -1)
    REGISTER_SETGET(green, -1)
    REGISTER_SETGET(blue, -1)

    REGISTER_NULLABLE
    REGISTER_JSONABLE
  }

  void Tech::init(int vr, int vg, int vb) {
    set_ptr(new ::Tech(vr, vg, vb));
  }

  void Tech::increment(Colour *c) {
    _ptr->increment(*c->_ptr);
  }

  void Tech::add(Tech *other) {
    _ptr->add(*other->_ptr);
  }

  bool Tech::includes(Tech *other) {
    return _ptr->includes(*other->_ptr);
  }

  IMPL_SETGET(int, red);
  IMPL_SETGET(int, green);
  IMPL_SETGET(int, blue);

  IMPL_NULLABLE
  IMPL_JSONABLE
}
