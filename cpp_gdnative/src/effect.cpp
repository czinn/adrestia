#include "effect.h"

#include "sticky_invoker.h"

#define CLASSNAME Effect

using namespace godot;

namespace godot {
	SCRIPT_AT("res://native/effect.gdns")

	void Effect::_register_methods() {
		REGISTER_METHOD(get_kind)
		REGISTER_METHOD(get_targets_self)
		REGISTER_METHOD(get_effect_type)
		REGISTER_METHOD(get_sticky_invoker)
		REGISTER_NULLABLE
		REGISTER_JSONABLE
	}

	FORWARD_AUTO_GETTER(get_kind)
	FORWARD_AUTO_GETTER(get_targets_self)
	FORWARD_AUTO_GETTER(get_effect_type)
	FORWARD_AUTO_GETTER(get_amount)
	FORWARD_AUTO_GETTER(get_sticky_invoker)

	IMPL_NULLABLE
	IMPL_JSONABLE
}
