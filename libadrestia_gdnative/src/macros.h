#pragma once
#include <Godot.hpp>
#include <JSONParseResult.hpp>
#include <JSON.hpp>

template<typename T>
godot::Ref<godot::JSONParseResult> to_godot_json(T &t) {
  nlohmann::json j = t;
  godot::String str = j.dump().c_str();
  return godot::JSON::parse(str);
}

// TO_JSONABLE:
// JSONParseResult as_json()

#define REGISTER_TO_JSONABLE(Thing)\
  register_method("as_json", &Thing::as_json);

#define INTF_TO_JSONABLE\
  Variant as_json() const;

#define IMPL_TO_JSONABLE(Thing, thing)\
  Variant Thing::as_json() const {\
    return to_godot_json(thing);\
  }

// JSONABLE:
// TO_JSONABLE
// void load_json_string(String)

#define REGISTER_JSONABLE(Thing)\
  register_method("load_json_string", &Thing::load_json_string);\
  REGISTER_TO_JSONABLE(Thing)

#define INTF_JSONABLE\
  void load_json_string(String str);\
  INTF_TO_JSONABLE

#define IMPL_JSONABLE(Thing, thing)\
  void Thing::load_json_string(String str) {\
    std::string s(str.ascii().get_data());\
    thing = nlohmann::json::parse(s);\
  }\
  IMPL_TO_JSONABLE(Thing, thing)
