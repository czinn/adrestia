#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <effect.h>

#include "macros.h"

namespace godot {
	class Effect : public Reference, public Forwarder<::Effect, Effect> {
			GODOT_CLASS(Effect, Reference)
		public:
			static const char *resource_path;
			static void _register_methods();

			Variant get_kind();
			Variant get_targets_self();
			Variant get_effect_type();
			Variant get_amount();
			Variant get_sticky_invoker();

			INTF_NULLABLE
			INTF_JSONABLE
	};
}

MAKE_INSTANCEABLE(Effect)
