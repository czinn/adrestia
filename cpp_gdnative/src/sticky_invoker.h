#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <sticky_invoker.h>

#include "macros.h"

namespace godot {
	class Duration;

	class StickyInvoker : public Reference, public Forwarder<::StickyInvoker, StickyInvoker> {
			GODOT_CLASS(StickyInvoker, Reference)
		public:
			static const char *resource_path;
			static void _register_methods();

			Variant get_sticky_id();
			Variant get_amount();
			Variant get_duration();

			INTF_NULLABLE
			INTF_JSONABLE
	};
}

MAKE_INSTANCEABLE(StickyInvoker)
