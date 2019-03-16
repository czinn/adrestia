#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <player.h>

#include "macros.h"

namespace godot {
	class EffectInstance;
	class Spell;

	class Player : public Reference, public Forwarder<::Player, Player> {
			GODOT_CLASS(Player, Reference)
		public:
			static const char *resource_path;
			static void _register_methods();
			
			// TODO: charles: Implement find_spell if needed
			
			int find_book_idx(String book_id);
			Variant level();
			Variant pipe_effect(EffectInstance *effect, bool inbound);
			Variant pipe_spell(Spell *spell);
			Variant pipe_turn();

			void subtract_step();
			void subtract_turn();

			INTF_SETGET(int, id)
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
