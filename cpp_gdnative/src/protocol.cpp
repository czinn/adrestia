#include "protocol.h"

#include "../../server/adrestia_networking.h"

#define CLASSNAME Protocol

using namespace godot;

namespace godot {
  Protocol::Protocol() {
  }

  Protocol::~Protocol() {
  }

  void Protocol::_register_methods() {
  }

  String Protocol::make_floop_request() {
    nlohmann::json j;
    String s(j.dump().c_str());
    return s;
  }
}
