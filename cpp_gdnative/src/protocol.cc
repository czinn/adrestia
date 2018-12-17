#include "protocol.h"

#include "../../server/protocol.h"

#define CLASSNAME Protocol

using namespace godot;

Protocol::Protocol() {
}

Protocol::~Protocol() {
}

void Protocol::_register_methods() {
  register_method("make_floop_request", &Protocol::make_floop_request);
}

String Protocol::make_floop_request() {
  nlohmann::json j;
  write_floop_request(j);
  String s(j.dump().c_str());
  return s;
}
