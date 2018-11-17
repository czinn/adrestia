#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <sticky_invoker.h>

#include "duration.h"
#include "macros.h"

namespace godot {
	class StickyInvoker : public godot::GodotScript<Reference>,
			public OwnerOrPointer<::StickyInvoker> {
			GODOT_CLASS(StickyInvoker)
		private:
			godot::Ref<godot::NativeScript> Duration_;
		public:
			StickyInvoker();
			static const char *resource_path;
			static void _register_methods();

			String get_sticky_id() const;
			int get_amount() const;
			Variant get_duration() const;

			INTF_NULLABLE
			INTF_JSONABLE
	};
}
