#pragma once
#include <Godot.hpp>
#include <NativeScript.hpp>
#include <ResourceLoader.hpp>
#include <JSONParseResult.hpp>
#include <JSON.hpp>

// Some metaprogramming magic to allow arity overloading macros.
// From https://stackoverflow.com/questions/16683146/can-macros-be-overloaded-by-number-of-arguments
#define CAT( A, B ) A ## B
#define SELECT( NAME, NUM ) CAT( NAME ## _, NUM )
#define GET_COUNT( _1, _2, _3, _4, _5, _6 /* ad nauseam */, COUNT, ... ) COUNT
#define VA_SIZE( ... ) GET_COUNT( __VA_ARGS__, 6, 5, 4, 3, 2, 1 )
#define VA_SELECT( NAME, ... ) SELECT( NAME, VA_SIZE(__VA_ARGS__) )(__VA_ARGS__)

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
  // Owner of the data. If null, I own the data.
  godot::Ref<godot::Reference> _owner;
 public:
  T *_ptr; // Pointer to the underlying data.
  inline void del_ptr() {
    if (_ptr != nullptr && _owner.is_null()) {
      //godot::Godot::print("Pointer deleted");
      //godot::Godot::print(godot::String(nlohmann::json(*_ptr).dump().c_str()));
      delete _ptr;
    }
  }

  OwnerOrPointer() : _ptr(nullptr), _owner(nullptr) {
    //godot::Godot::print("Thing instantiated");
  }
  ~OwnerOrPointer() {
    //godot::Godot::print("Thing deinstantiated");
    del_ptr();
  }

  OwnerOrPointer &operator=(const OwnerOrPointer &other) {
    // jim: Extremely fast failure if the copy assignment operator is ever
    // called. If we decide we actually do want to use it, uncomment the
    // following line and it should probably work?
    abort();

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
    //godot::Godot::print("Pointer set (owning)");
  }

  // Don't own it.
  void set_ptr(T *u, godot::Reference *r) {
    del_ptr();
    _ptr = u;
    _owner = godot::Ref<godot::Reference>(r);
    //godot::Godot::print("Pointer set (non-owning)");
  }
};

// jim: So it turns out instantiating other scripts from within a
// nativescript is kinda hard. Our GodotScripts need owners which have to be
// created by Godot, so we need a handle to the NativeScript object (the
// thing we normally call .new() on from gdscript). That means either:
// - methods that instantiate other scripts need to be passed a NativeScript*
// for every other thing they instantiate (yuck)
// OR:
// - we need to load these NativeScripts through ResourceLoader and make them
// available to the class as a member (we do this for now)
template<class T>
std::pair<godot::Variant, T*> instance(godot::Ref<godot::NativeScript> native_script_) {
  godot::Variant v = native_script_->call("new");
  T *t = godot::as<T>(v);
  return std::make_pair(v, t);
}

#define REGISTER_METHOD(method)\
  register_method(#method, &CLASSNAME::method);

#define FORWARD_STRING_GETTER(getter)\
  String CLASSNAME::getter() const {\
    return String(_ptr->getter().c_str());\
  }

#define FORWARD_GETTER(type, getter)\
  type CLASSNAME::getter() const {\
    return _ptr->getter();\
  }

#define FORWARD_ARRAY_GETTER(getter)\
  Array CLASSNAME::getter() const {\
    Array result;\
    for (const auto &x : _ptr->getter()) {\
      result.append(x);\
    }\
    return result;\
  }

// Implied naming convention:
// Member handles to Ref<NativeScript> for Kind are named Kind_
#define FORWARD_SMART_PTR_GETTER(Kind, getter)\
  Variant CLASSNAME::getter() const {\
    auto [v, kind] = instance<Kind>(Kind ## _);\
    kind->set_ptr(_ptr->getter().get(), owner);\
    return v;\
  }

#define FORWARD_REF_GETTER(Kind, getter)\
  Variant CLASSNAME::getter() const {\
    auto [v, kind] = instance<Kind>(Kind ## _);\
    kind->set_ptr(const_cast<::Kind*>(&_ptr->getter()), owner);\
    return v;\
  }

#define FORWARD_REF_ARRAY_GETTER(Kind, getter)\
  Array CLASSNAME::getter() const {\
    Array result;\
    for (const auto &x : _ptr->getter()) {\
      auto [v, thing] = instance<Kind>(Kind ## _);\
      thing->set_ptr(const_cast<::Kind*>(&x), owner);\
      result.append(v);\
    }\
    return result;\
  }

// SETGET(Type, member):
// Type get_member()
// void set_member(Type x)
#define REGISTER_SETGET(member, default_value)\
  register_property(#member, &CLASSNAME::set_ ## member, &CLASSNAME::get_ ## member, default_value);

#define INTF_SETGET(Type, member)\
  Type get_ ## member() const;\
  void set_ ## member(Type value);

#define IMPL_SETGET(Type, member)\
  Type CLASSNAME::get_ ## member() const {\
    return _ptr->member;\
  }\
  void CLASSNAME::set_ ## member(Type x) {\
    _ptr->member = x;\
  }

// NULLABLE:
// bool is_null()
// bool not_null()

#define REGISTER_NULLABLE\
  register_method("is_null", &CLASSNAME::is_null);\
  register_method("not_null", &CLASSNAME::not_null);

#define INTF_NULLABLE\
  bool is_null() const;\
  bool not_null() const;

#define IMPL_NULLABLE\
  bool CLASSNAME::is_null() const {\
    return _ptr == nullptr;\
  }\
  bool CLASSNAME::not_null() const {\
    return _ptr != nullptr;\
  }

// TO_JSONABLE:
// JSONParseResult as_json()

#define REGISTER_TO_JSONABLE\
  register_method("as_json", &CLASSNAME::as_json);

#define INTF_TO_JSONABLE\
  Variant as_json() const;

#define IMPL_TO_JSONABLE\
  Variant CLASSNAME::as_json() const {\
    return to_godot_json(*_ptr);\
  }

// JSONABLE: JSONABLE and
// void load_json_string(String)

#define REGISTER_JSONABLE\
  register_method("load_json_string", &CLASSNAME::load_json_string);\
  REGISTER_TO_JSONABLE

#define INTF_JSONABLE\
  void load_json_string(String str);\
  INTF_TO_JSONABLE

#define IMPL_JSONABLE\
  void CLASSNAME::load_json_string(String str) {\
    ::CLASSNAME *tmp = new ::CLASSNAME();\
    *tmp = nlohmann::json::parse(str.utf8().get_data());\
    set_ptr(tmp);\
  }\
  IMPL_TO_JSONABLE
