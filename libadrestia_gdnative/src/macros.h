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

// An OwnerOrPointer<T> holds a pointer _ptr that it may own, or that may be
// owned by some other Godot object _owner.
// If the _owner is null, we know that we ourselves own _ptr and must free it.
// Otherwise, another Godot object owns it and we hold the _owner Ref to them
// to make sure they don't free it.
template<typename T>
class OwnerOrPointer {
 private:
  godot::Ref<godot::Reference> _owner;
 public:
  T *_ptr;
  inline void del_ptr() {
    if (_ptr != nullptr && _owner.is_null()) {
      delete _ptr;
    }
  }

  OwnerOrPointer() : _ptr(nullptr), _owner(nullptr) {}
  ~OwnerOrPointer() { del_ptr(); }

  OwnerOrPointer &operator=(const OwnerOrPointer &other) {
    // jim: Extremely fast failure if the copy assignment operator is ever
    // called. If we decide we actually do want to use it, uncomment the
    // following line and it should work.
    abort();
    assert(false);
    _owner = other._owner;
    if (other._ptr != nullptr && _owner.is_null()) {
      _ptr = new T();
      *_ptr = *other._ptr;
    } else {
      _ptr = other._ptr;
    }
  }

  // Own it.
  void set_ptr(T *u) {
    del_ptr();
    _ptr = u;
    _owner.unref();
  }

  // Don't own it.
  void set_ptr(T *u, godot::Reference *r) {
    del_ptr();
    _ptr = u;
    _owner = r;
  }
};

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
    ::Thing *tmp = new ::Thing();\
    *tmp = nlohmann::json::parse(s);\
    set_ptr(tmp);\
  }\
  IMPL_TO_JSONABLE(Thing, thing)
