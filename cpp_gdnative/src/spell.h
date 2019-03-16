#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <spell.h>

#include "macros.h"

namespace godot {
	class Spell : public Reference, public Forwarder<::Spell, Spell> {
			GODOT_CLASS(Spell, Reference)
		public:
			static const char *resource_path;
			static void _register_methods();

			Variant get_id();
			Variant get_name();
			Variant get_book();
			Variant get_tech();
			Variant get_level();
			Variant get_cost();
			Variant get_text();
			Variant get_effects();
			Variant is_counterspell();
			Variant is_tech_spell();
			Variant get_counterspell_selector();

			INTF_NULLABLE
			INTF_JSONABLE
	};
}

MAKE_INSTANCEABLE(Spell)
