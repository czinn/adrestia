#include "sticky_invoker.h"

#include "duration.h"

#define CLASSNAME StickyInvoker

using namespace godot;

namespace godot {
	SCRIPT_AT("res://native/sticky_invoker.gdns")

	void StickyInvoker::_register_methods() {
		REGISTER_METHOD(get_sticky_id);
		REGISTER_METHOD(get_amount);
		REGISTER_METHOD(get_duration);
		REGISTER_NULLABLE
		REGISTER_JSONABLE
	}

	FORWARD_AUTO_GETTER(get_sticky_id)
	FORWARD_AUTO_GETTER(get_amount)
	FORWARD_AUTO_GETTER(get_duration)

	IMPL_NULLABLE
	IMPL_JSONABLE
}
