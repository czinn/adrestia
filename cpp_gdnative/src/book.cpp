#include "book.h"

#define CLASSNAME Book

using namespace godot;

namespace godot {
	const char *Book::resource_path = "res://native/book.gdns";

	void Book::_register_methods() {
		REGISTER_METHOD(get_id);
		REGISTER_METHOD(get_name);
		REGISTER_METHOD(get_spells);
		REGISTER_NULLABLE
		REGISTER_JSONABLE
	}

	FORWARD_STRING_GETTER(get_id)
	FORWARD_STRING_GETTER(get_name)
	FORWARD_STRING_ARRAY_GETTER(get_spells)

	IMPL_NULLABLE
	IMPL_JSONABLE
}
