#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <book.h>

#include "macros.h"

namespace godot {
	class Book : public Reference, public Forwarder<::Book, Book> {
			GODOT_CLASS(Book, Reference)
		public:
			static const char *resource_path;
			static void _register_methods();

			Variant get_id();
			Variant get_name();
			Variant get_spells();

			INTF_NULLABLE
			INTF_JSONABLE
	};
}

MAKE_INSTANCEABLE(Book)
