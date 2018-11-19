#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <player.h>

#include "macros.h"

namespace godot {
	class Book;
	class EffectInstance;
	class GameRules;
	class Spell;
	class StickyInstance;

	class Player : public godot::GodotScript<Reference>,
			public OwnerOrPointer<::Player> {
			GODOT_CLASS(Player)
		private:
			godot::Ref<godot::NativeScript> Book_;
			godot::Ref<godot::NativeScript> EffectInstance_;
			godot::Ref<godot::NativeScript> GameRules_;
			godot::Ref<godot::NativeScript> Spell_;
			godot::Ref<godot::NativeScript> StickyInstance_;
		public:
			Player();
			static const char *resource_path;
			static void _register_methods();
			
			// TODO: charles: Implement find_spell if needed
			
			int find_book_idx(String book_id) const;
			Variant level() const;
			Array pipe_effect(int player_id, EffectInstance *effect, bool inbound);
			Array pipe_spell(int player_id, Spell *spell);
			Array pipe_turn(int player_id);

			void subtract_step();
			void subtract_turn();

			INTF_SETGET(int, hp)
			INTF_SETGET(int, max_hp)
			INTF_SETGET(int, mp)
			INTF_SETGET(int, mp_regen)
			INTF_SETGET(Array, tech)
			INTF_SETGET(Array, books)
			INTF_SETGET(Array, stickies)

			INTF_NULLABLE
			INTF_TO_JSONABLE
	};
}
