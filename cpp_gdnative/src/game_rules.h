#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <game_rules.h>

#include "macros.h"

namespace godot {
	class Book;
	class Spell;
	class Sticky;

	class GameRules : public godot::GodotScript<Reference>, public Forwarder<::GameRules, GameRules> {
			GODOT_CLASS(GameRules)
		public:
			static const char *resource_path;
			static void _register_methods();

			Variant get_sticky(String id) const;
			Variant get_spell(String id) const;
			Variant get_book(String id) const;
			Variant get_books() const;
			Variant get_mana_cap() const;
			Variant get_initial_health() const;
			Variant get_initial_mana_regen() const;

			INTF_NULLABLE
			INTF_JSONABLE
	};
}

MAKE_INSTANCEABLE(GameRules)
