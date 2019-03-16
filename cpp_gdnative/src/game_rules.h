#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <game_rules.h>

#include "macros.h"

namespace godot {
	class Book;
	class Spell;
	class Sticky;

	class GameRules : public Reference, public Forwarder<::GameRules, GameRules> {
			GODOT_CLASS(GameRules, Reference)
		public:
			static const char *resource_path;
			static void _register_methods();

			Variant get_version();
			Variant get_sticky(String id);
			Variant get_spell(String id);
			Variant get_book(String id);
			Variant get_books();
			Variant get_mana_cap();
			Variant get_initial_health();
			Variant get_initial_mana_regen();

			INTF_NULLABLE
			INTF_JSONABLE
	};
}

MAKE_INSTANCEABLE(GameRules)
