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

  IMPL_JSONABLE(Tech, *_ptr)
}
