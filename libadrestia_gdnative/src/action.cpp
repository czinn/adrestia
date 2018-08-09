#include "action.h"

using namespace godot;

namespace godot {
  void Action::_register_methods() {
    register_method("init_tech_colour", &Action::init_tech_colour);
    register_method("init_units", &Action::init_units);
    REGISTER_JSONABLE(Action)
  }

  void Action::init_tech_colour(Colour *colour) {
    _action = std::make_shared<::Action>(colour->_colour);
  }

  void Action::init_units(Array units) {
    std::vector<std::string> units_;
    for (int i = 0; i < units.size(); i++) {
      String s = units[i];
      units_.push_back(std::string(s.ascii().get_data()));
    }
    _action = std::make_shared<::Action>(units_);
  }

  IMPL_JSONABLE(Action, *_action)
}
