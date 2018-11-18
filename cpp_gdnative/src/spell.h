#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <spell.h>

#include "macros.h"

namespace godot {
	class Spell : public godot::GodotScript<Reference>, public OwnerOrPointer<::Spell> {
			GODOT_CLASS(Spell)
		private:
			godot::Ref<godot::NativeScript> Selector_;
			godot::Ref<godot::NativeScript> Effect_;
		public:
			Spell();
			static const char *resource_path;
			static void _register_methods();

			String get_id() const;
			String get_name() const;
			String get_book() const;
			int get_tech() const;
			int get_level() const;
			int get_cost() const;
			String get_text() const;
			Array get_effects() const;
			bool is_counterspell() const;
			bool is_tech_spell() const;
			Variant get_counterspell_selector() const;

			INTF_NULLABLE
			INTF_JSONABLE
	};
}
