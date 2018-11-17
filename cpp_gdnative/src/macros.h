#pragma once
#include <Godot.hpp>
#include <NativeScript.hpp>
#include <ResourceLoader.hpp>
#include <JSONParseResult.hpp>
#include <JSON.hpp>

// Work around a peculiarity in the macro replacement algorithm.
// https://stackoverflow.com/questions/12648988/converting-a-defined-constant-number-to-a-string
#define STRINGIZE_(x) #x
#define STRINGIZE(x) STRINGIZE_(x)

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
template<class T>
class OwnerOrPointer {
 private:
  // Owner of the data. If null, I own the data.
  godot::Ref<godot::Reference> _owner;

 public:
  T *_ptr; // Pointer to the underlying data.
  inline void del_ptr() {
    if (_ptr != nullptr && _owner.is_null()) {
      delete _ptr;
    }
  }

  OwnerOrPointer() : _ptr(nullptr), _owner(nullptr) {
  }
  ~OwnerOrPointer() {
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
  }

  // Don't own it.
  void set_ptr(T *u, godot::Reference *r) {
    del_ptr();
    _ptr = u;
    _owner = godot::Ref<godot::Reference>(r);
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

// TODO: jim: Can we instance our classes by storing a single static
// Ref<NativeScript> for each class instead of holding Ref<NativeScript> in
// every instance that needs one? The answer seems to be "yes, but you'll get
// ugly warning because those refs stop the NativeScripts from ever being
// collected". Also, instead of the following approach we would probably have
// to use macros, because you can't instantiate a class's parent's template
// with the class itself.
// FOLLOWING CODE IS NOT USED
template<class T>
class Instanceable {
  static const char *resource_path;
  static godot::Ref<godot::NativeScript> native_script;
  std::pair<godot::Variant, T*> instance() {
    if (native_script.is_null()) {
      native_script = godot::ResourceLoader::load(resource_path);
    }
    godot::Variant v = native_script->call("new");
    T *t = godot::as<T>(v);
    return std::make_pair(v, t);
  }
  ~Instanceable() {
    // jim: We don't actually need to dereference the NativeScript whenever an
    // instance of it is destroyed. But without this, we get the following ugly
    // Godot error in the logs when exiting the game:
    //   WARNING: cleanup: ObjectDB Instances still exist!
    //   At: core/object.cpp:1989.
    //   ERROR: clear: Resources Still in use at Exit!
    //   At: core/resource.cpp:418.
    // i.e. holding a static reference to the NativeScript prevents it from
    // being destroyed when godot exits. So for now, we dereference the
    // nativescript.
    native_script.unref();
  }
};

#define REGISTER_METHOD(method)\
  register_method(#method, &CLASSNAME::method);

#define FORWARD_VOID(method)\
	void CLASSNAME::method() {\
		_ptr->method();\
	}

#define FORWARD_GETTER(type, getter)\
  type CLASSNAME::getter() const {\
    return _ptr->getter();\
  }

#define FORWARD_STRING_GETTER(getter)\
  String CLASSNAME::getter() const {\
    return String(_ptr->getter().c_str());\
  }

#define FORWARD_ARRAY_GETTER(getter)\
  Array CLASSNAME::getter() const {\
    Array result;\
    for (const auto &x : _ptr->getter()) {\
      result.append(x);\
    }\
    return result;\
  }

#define FORWARD_STRING_ARRAY_GETTER(getter)\
	Array CLASSNAME::getter() const {\
		Array result;\
		for (const auto &x : _ptr->getter()) {\
			result.append(String(x.c_str()));\
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

// SETGET_REF:
// Implements SETGET for a mutable class member.
// Requires INTF_SETGET(Variant, ...)
#define IMPL_SETGET_REF(Type, member)\
  Variant CLASSNAME::get_ ## member() const {\
    auto [v, thing] = instance<Type>(Type ## _);\
    thing->set_ptr(const_cast<::Type*>(&_ptr->member), owner);\
    return v;\
  }\
  void CLASSNAME::set_ ## member(Variant value) {\
    Tech *thing = godot::as<Type>(value);\
    _ptr->tech = *thing->_ptr;\
  }

// SETGET_CONST_REF:
// Implements SETGET for an immutable class member.
// Requires INTF_SETGET(Variant, ...)
#define IMPL_SETGET_CONST_REF(Type, member)\
  Variant CLASSNAME::get_ ## member() const {\
    auto [v, thing] = instance<Type>(Type ## _);\
    thing->set_ptr(const_cast<::Type*>(&_ptr->member), owner);\
    return v;\
  }\
  void CLASSNAME::set_ ## member(Variant value) {\
    Godot::print("Error: Called " STRINGIZE(CLASSNAME) "::set_" #member " (setter for a const member)");\
    assert(false);\
  }

// SETGET_REF_DICT:
// Implements SETGET for an std::map-like class member. Only implements the
// getter; the setter is left undefined because why would you ever want to set
// a dict from gdscript. Key type must be implicitly convertible to Variant
// (string won't work).
#define IMPL_SETGET_REF_DICT(Type, member)\
  Variant CLASSNAME::get_ ## member() const {\
    Dictionary d;\
    for (const auto &[k, x] : _ptr->member) {\
      auto [v, thing] = instance<Type>(Type ## _);\
      thing->set_ptr(const_cast<::Type*>(&x), owner);\
      d[k] = v;\
    }\
    return d;\
  }\
  void CLASSNAME::set_ ## member(Variant v) {\
    Godot::print("Error: Called " STRINGIZE(CLASSNAME) "::set_" #member " (setter for a dict member)");\
    assert(false);\
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
