#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <effect.h>

#include "macros.h"

namespace godot {
	class Effect : public godot::GodotScript<Reference>, public Forwarder<::Effect, Effect> {
			GODOT_CLASS(Effect)
		public:
			static const char *resource_path;
			static void _register_methods();

			Variant get_kind() const;
			Variant get_targets_self() const;
			Variant get_effect_type() const;
			Variant get_amount() const;
			Variant get_sticky_invoker() const;

			INTF_NULLABLE
			INTF_JSONABLE
	};
}

MAKE_INSTANCEABLE(Effect)
