#include <sstream>
#include "tech.h"
#include "macros.h"

using namespace godot;

namespace godot {
  void Tech::_register_methods() {
    register_method("add", &godot::Tech::add);
    register_method("init", &godot::Tech::init);
    REGISTER_JSONABLE(Tech)
  }

  void Tech::init(int vr, int vg, int vb) {
    _tech = ::Tech(vr, vg, vb);
  }

  void Tech::increment(Colour *c) {
    _tech.increment(c->_colour);
  }

  void Tech::add(Tech *other) {
    _tech.add(other->_tech);
  }

  bool Tech::includes(Tech *other) {
    return _tech.includes(other->_tech);
  }

  IMPL_JSONABLE(Tech, _tech)
}
