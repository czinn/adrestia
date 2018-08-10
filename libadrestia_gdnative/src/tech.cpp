#include <sstream>
#include "tech.h"
#include "macros.h"

using namespace godot;

namespace godot {
  void Tech::_register_methods() {
    register_method("add", &godot::Tech::add);
    register_method("as_json", &godot::Tech::as_json);
    register_method("init", &godot::Tech::init);
  }

  void Tech::init(int vr, int vg, int vb) {
    _tech = ::Tech(vr, vg, vb);
  }

  void Tech::increment(Colour *c) {
    _tech.increment(c->_colour);
  }

  void Tech::add(Tech *v) {
    _tech.add(v->_tech);
  }

  Variant Tech::as_json() {
    return to_godot_json(_tech);
  }
}
