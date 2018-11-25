#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <player.h>

#include "macros.h"

namespace godot {
	class EffectInstance;
	class Spell;

	class Player : public godot::GodotScript<Reference>, public Forwarder<::Player, Player> {
			GODOT_CLASS(Player)
		public:
			static const char *resource_path;
			static void _register_methods();
			
			// TODO: charles: Implement find_spell if needed
			
			int find_book_idx(String book_id) const;
			Variant level() const;
			Variant pipe_effect(int player_id, EffectInstance *effect, bool inbound);
			Variant pipe_spell(int player_id, Spell *spell);
			Variant pipe_turn(int player_id);

			void subtract_step();
			void subtract_turn();

			INTF_SETGET(int, hp)
			INTF_SETGET(int, max_hp)
			INTF_SETGET(int, mp)
			INTF_SETGET(int, mp_regen)
			INTF_SETGET(Variant, tech)
			INTF_SETGET(Variant, books)
			INTF_SETGET(Variant, stickies)

			INTF_NULLABLE
			INTF_TO_JSONABLE
	};
}

MAKE_INSTANCEABLE(Player)
