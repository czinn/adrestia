#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <spell.h>

#include "macros.h"

namespace godot {
	class Spell : public godot::GodotScript<Reference>, public Forwarder<::Spell, Spell> {
			GODOT_CLASS(Spell)
		private:
			godot::Ref<godot::NativeScript> Selector_;
			godot::Ref<godot::NativeScript> Effect_;
		public:
			Spell();
			static const char *resource_path;
			static void _register_methods();

			Variant get_id() const;
			Variant get_name() const;
			Variant get_book() const;
			Variant get_tech() const;
			Variant get_level() const;
			Variant get_cost() const;
			Variant get_text() const;
			Array get_effects() const;
			Variant is_counterspell() const;
			Variant is_tech_spell() const;
			Variant get_counterspell_selector() const;

			INTF_NULLABLE
			INTF_JSONABLE
	};
}
