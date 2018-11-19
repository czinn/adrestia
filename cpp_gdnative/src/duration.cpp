#include "duration.h"

#define CLASSNAME Duration

using namespace godot;

namespace godot {
	const char *Duration::resource_path = "res://native/duration.gdns";

	void Duration::_register_methods() {
		REGISTER_METHOD(is_active);
		REGISTER_METHOD(get_unit);
		REGISTER_METHOD(get_value);
		REGISTER_METHOD(subtract_step);
		REGISTER_METHOD(subtract_turn);
		REGISTER_METHOD(set_to_zero);
		REGISTER_NULLABLE
		REGISTER_JSONABLE
	}

	FORWARD_AUTO_GETTER(is_active)
	FORWARD_AUTO_GETTER(get_unit)
	FORWARD_AUTO_GETTER(get_value)

	FORWARD_VOID(subtract_step)
	FORWARD_VOID(subtract_turn)
	FORWARD_VOID(set_to_zero)

	IMPL_NULLABLE
	IMPL_JSONABLE
}
