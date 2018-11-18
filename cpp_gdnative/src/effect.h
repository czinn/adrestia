#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <effect.h>

#include "macros.h"

namespace godot {
	class Effect : public godot::GodotScript<Reference>, public OwnerOrPointer<::Effect> {
			GODOT_CLASS(Effect)
		private:
			godot::Ref<godot::NativeScript> StickyInvoker_;
		public:
			Effect();
			static const char *resource_path;
			static void _register_methods();

			int get_kind() const;
			bool get_targets_self() const;
			int get_effect_type() const;
			int get_amount() const;
			Variant get_sticky_invoker() const;

			INTF_NULLABLE
			INTF_JSONABLE
	};
}
