#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <sticky_invoker.h>

#include "macros.h"

namespace godot {
	class Duration;

	class StickyInvoker : public godot::GodotScript<Reference>,
			public Forwarder<::StickyInvoker, StickyInvoker> {
			GODOT_CLASS(StickyInvoker)
		public:
			static const char *resource_path;
			static void _register_methods();

			Variant get_sticky_id() const;
			Variant get_amount() const;
			Variant get_duration() const;

			INTF_NULLABLE
			INTF_JSONABLE
	};
}

MAKE_INSTANCEABLE(StickyInvoker)
