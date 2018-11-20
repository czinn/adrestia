#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <game_state.h>
#include <game_action.h>

#include "macros.h"

namespace godot {
	class GameState : public godot::GodotScript<Reference>, public Forwarder<::GameState, GameState> {
			GODOT_CLASS(GameState)
		public:
			static const char *resource_path;
			static void _register_methods();

			void init(Variant rules, Variant player_books);
			bool is_valid_action(int player_id, Variant action) const;
			Variant turn_number() const;
			Variant winners() const;
			bool simulate(Variant actions);

			INTF_SETGET(Variant, history)
			INTF_SETGET(Variant, players)
			INTF_SETGET(Variant, rules)

			INTF_NULLABLE
			INTF_TO_JSONABLE
	};
}

MAKE_INSTANCEABLE(GameState)
