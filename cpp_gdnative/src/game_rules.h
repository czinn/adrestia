#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <game_rules.h>

#include "macros.h"

namespace godot {
	class Book;
	class Spell;
	class Sticky;

	class GameRules : public godot::GodotScript<Reference>, public OwnerOrPointer<::GameRules> {
			GODOT_CLASS(GameRules)
		private:
			godot::Ref<godot::NativeScript> Book_;
			godot::Ref<godot::NativeScript> Spell_;
			godot::Ref<godot::NativeScript> Sticky_;
		public:
			GameRules();
			static const char *resource_path;
			static void _register_methods();

			Variant get_sticky(String id) const;
			Variant get_spell(String id) const;
			Variant get_book(String id) const;
			Dictionary get_books();
			int get_mana_cap() const;
			int get_initial_health() const;
			int get_initial_mana_regen() const;

			INTF_NULLABLE
			INTF_JSONABLE
	};
}
