#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <duration.h>

#include "macros.h"

namespace godot {
	class Duration : public Reference, public Forwarder<::Duration, Duration> {
			GODOT_CLASS(Duration, Reference)
		public:
			static const char *resource_path;
			static void _register_methods();

			Variant is_active();
			Variant get_unit();
			Variant get_value();

			void subtract_step();
			void subtract_turn();
			void set_to_zero();

			INTF_NULLABLE
			INTF_JSONABLE
	};
}

MAKE_INSTANCEABLE(Duration)
