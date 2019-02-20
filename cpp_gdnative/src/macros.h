#pragma once

#include <vector>
#include <list>

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

// A Forwarder<T, Self> provides two capabilities:
// - It holds a T* _ptr that it may own, or that may be owned by some other
// Godot object _owner. If the _owner is null, we know that we ourselves own
// _ptr and must free it. Otherwise, another Godot object owns it and we hold
// the _owner Ref to them to make sure they don't free it.
// - It allows instantiating itself through the Godot runtime using the static
// make_instance() method.
//
// Yes, classes can inherit from a template superclass instantiated with the
// class itself. Turns out this is a technique called CRTP.
namespace godot {
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
				std::cout << "DELYEET " << size_t(_ptr) << std::endl;
				delete _ptr;
			}
		}

		Forwarder() : _ptr(nullptr), _owner(nullptr) {
		}
		~Forwarder() {
			del_ptr();
			_native_script.unref();
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
			std::cout << "antic " << size_t(u) << std::endl;
			del_ptr();
			_ptr = u;
			std::cout << "CURYATE(owned) " << _resource_path << " " << size_t(_ptr) << std::endl;
			_owner.unref();
		}

		// Don't own it.
		void set_ptr(T *u, godot::Reference *r) {
			std::cout << "antic " << size_t(u) << std::endl;
			del_ptr();
			_ptr = u;
			std::cout << "CURYATE(borrowed) " << _resource_path << " " << size_t(_ptr) << std::endl;
			_owner = godot::Ref<godot::Reference>(r);
		}
};
}

// Converts ANY C++ type into a Godot variant.
// TODO: jim: This guy (https://stackoverflow.com/questions/39250545/) says
// template specialization is a trap. Is there a better way to achieve this
// effect?
template<class T>
inline godot::Variant to_godot_variant(T x, godot::Reference *owner) {
	return x;
}

template<class V>
inline godot::Variant to_godot_variant(const std::vector<V> &c, godot::Reference *owner) {
	godot::Array result;
	for (const auto &x : c) {
		result.append(to_godot_variant(x, owner));
	}
	return result;
}

template<class V>
inline godot::Variant to_godot_variant(const std::list<V> &c, godot::Reference *owner) {
	godot::Array result;
	for (const auto &x : c) {
		result.append(to_godot_variant(x, owner));
	}
	return result;
}

template<class K, class V>
inline godot::Variant to_godot_variant(const std::map<K, V> &m, godot::Reference *owner) {
	godot::Dictionary result;
	for (auto &[k, v] : m) {
		result[to_godot_variant(k, owner)] = to_godot_variant(v, owner);
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

template<>
inline godot::Variant to_godot_variant(unsigned long num, godot::Reference *owner) {
	return (unsigned int) num;
}

template<>
inline godot::Variant to_godot_variant(nlohmann::json j, godot::Reference *owner) {
	return godot::JSON::parse(j.dump().c_str())->get_result();
}

// of_godot_variant<T> "writes" a godot::Variant to the T*.
template<class T>
inline void of_godot_variant(godot::Variant v, T *t) {
	t->this_template_should_never_be_instantiated;
}

template<>
inline void of_godot_variant(godot::Variant v, std::string *t) {
	godot::String s = v;
	*t = std::string(s.ascii().get_data());
}

template<typename V>
inline void of_godot_variant(godot::Variant v, std::vector<V> *vec) {
	vec->clear();
	godot::Array a = v;
	for (size_t i = 0; i < a.size(); i++) {
		V elem;
		of_godot_variant(a[i], &elem);
		vec->push_back(elem);
	}
}

template<>
inline void of_godot_variant(godot::Variant v, nlohmann::json *j) {
	if (v.get_type() == godot::Variant::DICTIONARY || v.get_type() == godot::Variant::ARRAY) {
		std::string s;
		of_godot_variant(godot::JSON::print(v), &s);
		*j = nlohmann::json::parse(s);
	} else {
		abort();
	}
}

// jim: These cases are covered by the base template. However, I'm not sure the
// base template is a great idea, so I'm keeping this around just in case.
//template<> inline godot::Variant to_godot_variant(int x, godot::Reference *owner) { return x; }
//template<> inline godot::Variant to_godot_variant(bool x, godot::Reference *owner) { return x; }

#define SCRIPT_AT(path)\
	const char *CLASSNAME::resource_path = path;\
	template<> const char *Forwarder<::CLASSNAME, CLASSNAME>::_resource_path = path;\
	template<> Ref<NativeScript> Forwarder<::CLASSNAME, CLASSNAME>::_native_script = nullptr;

// TODO: jim: This may do unnecessary copying of things that could be passed as
// references. See if this is the case and fix it if so.
// (This can be extended to support wrapping smart pointers.)
#define MAKE_INSTANCEABLE(CLASSNAME)\
	template<>\
	inline godot::Variant to_godot_variant(const CLASSNAME *x, godot::Reference *owner) {\
		auto [w, o] = godot::CLASSNAME::make_instance();\
		o->set_ptr(const_cast<CLASSNAME*>(x), owner);\
		godot::Godot::print(godot::String("Wrapped pointer for " STRINGIZE(CLASSNAME) " at ") + godot::String::num_int64((size_t) o->_ptr));\
		return w;\
	}\
	template<>\
	inline godot::Variant to_godot_variant(const CLASSNAME x, godot::Reference *owner) {\
		auto [w, o] = godot::CLASSNAME::make_instance();\
		o->set_ptr(new ::CLASSNAME(x));\
		godot::Godot::print(godot::String("Wrapped value for " STRINGIZE(CLASSNAME) " at ") + godot::String::num_int64((size_t) o->_ptr));\
		return w;\
	}\
	template<>\
	inline godot::Variant to_godot_variant(const CLASSNAME &x, godot::Reference *owner) {\
		auto [w, o] = godot::CLASSNAME::make_instance();\
		o->set_ptr(const_cast<CLASSNAME*>(&x), owner);\
		godot::Godot::print(godot::String("Wrapped reference for " STRINGIZE(CLASSNAME) " at ") + godot::String::num_int64((size_t) o->_ptr));\
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

// jim: Using this MAY improve performance over the non-_REF version by not
// using the copying version of the template, but I'm not entirely sure
#define FORWARD_AUTO_GETTER_REF(getter)\
	Variant CLASSNAME::getter() const {\
		return to_godot_variant(&_ptr->getter(), owner);\
	}

#define FORWARD_REF_BY_ID_GETTER(Kind, getter)\
	Variant CLASSNAME::getter(String id) const {\
		const std::string key(id.ascii().get_data());\
		return to_godot_variant(&_ptr->getter(key), owner);\
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

// TODO: jim: Can we define a totally automatic mutable version of this?
#define IMPL_SETGET_CONST_AUTO(member)\
	Variant CLASSNAME::get_ ## member() const {\
		return to_godot_variant(_ptr->member, owner);\
	}\
	void CLASSNAME::set_ ## member(Variant value) {\
		Godot::print("Error: Called " STRINGIZE(CLASSNAME) "::set_" #member " (setter for a const auto member)");\
		assert(false);\
	}

#define IMPL_SETGET_REF(Type, member)\
	Type CLASSNAME::get_ ## member() const {\
		return to_godot_variant(_ptr->member, owner);\
	}\
	void CLASSNAME::set_ ## member(Variant value) {\
		_ptr->member = *thing->_ptr;\
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
