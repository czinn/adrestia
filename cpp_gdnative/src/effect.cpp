#include "effect.h"

#include "sticky_invoker.h"

#define CLASSNAME Effect

using namespace godot;

namespace godot {
	const char *Effect::resource_path = "res://native/effect.gdns";

	Effect::Effect() {
		StickyInvoker_ = ResourceLoader::load(StickyInvoker::resource_path);
	}

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
	FORWARD_REF_GETTER(StickyInvoker, get_sticky_invoker)

	IMPL_NULLABLE
	IMPL_JSONABLE
}
