#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <duration.h>

#include "macros.h"

namespace godot {
	class Duration : public godot::GodotScript<Reference>, public OwnerOrPointer<::Duration> {
			GODOT_CLASS(Duration)
		public:
			static const char *resource_path;
			static void _register_methods();

			bool is_active() const;
			int get_unit() const;
			int get_value() const;

			void subtract_step();
			void subtract_turn();
			void set_to_zero();

			INTF_NULLABLE
			INTF_JSONABLE
	};
}