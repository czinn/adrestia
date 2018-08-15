#include <sstream>
#include "tech.h"
#include "macros.h"

using namespace godot;

#define CLASSNAME Tech

namespace godot {
  const char *Tech::resource_path = "res://native/tech.gdns";

  void Tech::_register_methods() {
    register_method("good", &godot::Tech::good);
    register_method("init", &godot::Tech::init);
    register_method("increment", &godot::Tech::increment);
    register_method("add", &godot::Tech::add);
    register_method("includes", &godot::Tech::includes);
    REGISTER_JSONABLE
  }

  bool Tech::good() const { return _ptr != nullptr; }

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

  IMPL_JSONABLE
}
