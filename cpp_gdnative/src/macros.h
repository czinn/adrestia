#pragma once

#include <vector>

#include <Godot.hpp>
#include <NativeScript.hpp>
#include <ResourceLoader.hpp>
#include <JSONParseResult.hpp>
#include <JSON.hpp>

#include "../../cpp/json.h"

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

// A Forwarder<T, Self> holds a T* _ptr that it may own, or that may be owned
// by some other Godot object _owner.
// If the _owner is null, we know that we ourselves own _ptr and must free it.
// Otherwise, another Godot object owns it and we hold the _owner Ref to them
// to make sure they don't free it.
template<class T, class Self>
class Forwarder {
	private:
		static const char *_resource_path;
		static godot::Ref<godot::NativeScript> _native_script;
	public:
		inline static std::pair<godot::Variant, Self*> make_instance() {
			if (_native_script.is_null()) {
				_native_script = godot::ResourceLoader::load(_resource_path);
			}
			godot::Variant v = _native_script->call("new");
			Self *t = godot::as<Self>(v);
			return std::make_pair(v, t);
		}

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

		Forwarder() : _ptr(nullptr), _owner(nullptr) {
		}
		~Forwarder() {
			del_ptr();
			//_native_script.unref();
		}

		Forwarder &operator=(const Forwarder &other) {
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

// Instantiates a NativeScript.
template<class T>
std::pair<godot::Variant, T*> instance(godot::Ref<godot::NativeScript> native_script_) {
	godot::Variant v = native_script_->call("new");
	T *t = godot::as<T>(v);
	return std::make_pair(v, t);
}

template<class T>
inline godot::Variant to_godot_variant(T x, godot::Reference *owner) {
	return x;
}

template<class V>
inline godot::Variant to_godot_variant(const std::vector<V> &vec, godot::Reference *owner) {
	godot::Array result;
	for (const auto &x : vec) {
		result.append(to_godot_variant(x, owner));
	}
	return result;
}

template<>
inline godot::Variant to_godot_variant(const std::string str, godot::Reference *owner) {
	return godot::String(str.c_str());
}

template<>
inline godot::Variant to_godot_variant(const std::string &str, godot::Reference *owner) {
	return godot::String(str.c_str());
}

// jim: These cases are covered by the base template. However, I'm not sure the
// base template is a great idea, so I'm keeping this around just in case.
//template<> inline godot::Variant to_godot_variant(int x, godot::Reference *owner) { return x; }
//template<> inline godot::Variant to_godot_variant(bool x, godot::Reference *owner) { return x; }

#define SCRIPT_AT(path)\
	const char *CLASSNAME::resource_path = path;\
	template<> const char *Forwarder<::CLASSNAME, CLASSNAME>::_resource_path = path;\
	template<> Ref<NativeScript> Forwarder<::CLASSNAME, CLASSNAME>::_native_script = nullptr;

#define MAKE_INSTANCEABLE(CLASSNAME)\
	template<>\
	inline godot::Variant to_godot_variant(const CLASSNAME *x, godot::Reference *owner) {\
		auto [w, o] = godot::CLASSNAME::make_instance();\
		o->set_ptr(const_cast<CLASSNAME*>(x), owner);\
		return w;\
	}\
	template<>\
	inline godot::Variant to_godot_variant(const CLASSNAME &x, godot::Reference *owner) {\
		auto [w, o] = godot::CLASSNAME::make_instance();\
		o->set_ptr(const_cast<CLASSNAME*>(&x), owner);\
		return w;\
	}

#define REGISTER_METHOD(method)\
	register_method(#method, &CLASSNAME::method);

#define FORWARD_VOID(method)\
	void CLASSNAME::method() {\
		_ptr->method();\
	}

#define FORWARD_AUTO_GETTER(getter)\
	Variant CLASSNAME::getter() const {\
		return to_godot_variant(_ptr->getter(), owner);\
	}

#define FORWARD_AUTO_GETTER_REF(getter)\
	Variant CLASSNAME::getter() const {\
		return to_godot_variant(&_ptr->getter(), owner);\
	}

// Implied naming convention:
// Member handles to Ref<NativeScript> for Kind are named Kind_
#define FORWARD_SMART_PTR_GETTER(Kind, getter)\
	Variant CLASSNAME::getter() const {\
		auto [v, kind] = instance<Kind>(Kind ## _);\
		kind->set_ptr(const_cast<::Kind*>(_ptr->getter().get()), owner);\
		return v;\
	}

#define FORWARD_REF_GETTER(Kind, getter)\
	Variant CLASSNAME::getter() const {\
		auto [v, kind] = instance<Kind>(Kind ## _);\
		kind->set_ptr(const_cast<::Kind*>(&_ptr->getter()), owner);\
		return v;\
	}

#define FORWARD_REF_BY_ID_GETTER(Kind, getter)\
	Variant CLASSNAME::getter(String id) const {\
		auto [v, kind] = instance<Kind>(Kind ## _);\
		const std::string key(id.ascii().get_data());\
		kind->set_ptr(const_cast<::Kind*>(&_ptr->getter(key)), owner);\
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

#define IMPL_SETGET_ENUM(Type, member)\
	int CLASSNAME::get_ ## member() const {\
		return _ptr->member;\
	}\
	void CLASSNAME::set_ ## member(int x) {\
		_ptr->member = (Type)x;\
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
		Type *thing = godot::as<Type>(value);\
		_ptr->member = *thing->_ptr;\
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

// SETGET_ARRAY:
// Implements SETGET for an std::vector-like class member with basic contents.
// Only implements the getter; the setter is left undefined because why would
// you ever want to set an array from gdscript.
#define IMPL_SETGET_ARRAY(member)\
	Array CLASSNAME::get_ ## member() const {\
		return to_godot_variant(_ptr->member, owner);\
	}\
	void CLASSNAME::set_ ## member(Array v) {\
		Godot::print("Error: Called " STRINGIZE(CLASSNAME) "::set_" #member " (setter for an array member)");\
		assert(false);\
	}

// SETGET_REF_ARRAY:
// Implements SETGET for an std::vector-like class member. Only implements the
// getter; the setter is left undefined because why would you ever want to set
// an array from gdscript.
#define IMPL_SETGET_ARRAY_BASE(Type, member, x_)\
	Array CLASSNAME::get_ ## member() const {\
		Array result;\
		for (const auto &x : _ptr->member) {\
			auto [v, thing] = instance<Type>(Type ## _);\
			thing->set_ptr(const_cast<::Type*>(x_), owner);\
			result.append(v);\
		}\
		return result;\
	}\
	void CLASSNAME::set_ ## member(Array v) {\
		Godot::print("Error: Called " STRINGIZE(CLASSNAME) "::set_" #member " (setter for an array member)");\
		assert(false);\
	}

#define IMPL_SETGET_REF_ARRAY(Type, member) IMPL_SETGET_ARRAY_BASE(Type, member, &x)
#define IMPL_SETGET_PTR_ARRAY(Type, member) IMPL_SETGET_ARRAY_BASE(Type, member, x)

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
