#include "action.h"

using namespace godot;

#define CLASSNAME Action

namespace godot {
  void Action::_register_methods() {
    register_method("init_tech_colour", &Action::init_tech_colour);
    register_method("init_units", &Action::init_units);
    REGISTER_NULLABLE
    REGISTER_JSONABLE
  }

  void Action::init_tech_colour(Colour *colour) {
    set_ptr(new ::Action(*colour->_ptr));
  }

  void Action::init_units(Array units) {
    std::vector<std::string> units_;
    for (int i = 0; i < units.size(); i++) {
      String s = units[i];
      units_.push_back(std::string(s.ascii().get_data()));
    }
    set_ptr(new ::Action(units_));
  }

  IMPL_NULLABLE
  IMPL_JSONABLE
}
