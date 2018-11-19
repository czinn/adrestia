#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <book.h>

#include "macros.h"

namespace godot {
	class Book : public godot::GodotScript<Reference>, public OwnerOrPointer<::Book> {
			GODOT_CLASS(Book)
		public:
			static const char *resource_path;
			static void _register_methods();

			Variant get_id() const;
			Variant get_name() const;
			Variant get_spells() const;

			INTF_NULLABLE
			INTF_JSONABLE
	};
}
