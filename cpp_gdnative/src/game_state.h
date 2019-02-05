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
			void init_json(Variant rules, Variant json);
			void clone(Variant state);
			// Returns a game state not suitable for simulation, but suitable for apply_event
			void of_game_view(Variant view);
			bool simulate(Variant actions);
			Variant simulate_events(Variant actions);
			void apply_event(Variant event);

			bool is_valid_action(int player_id, Variant action) const;
			Variant turn_number() const;
			Variant winners() const;

			INTF_SETGET(Variant, history)
			INTF_SETGET(Variant, players)
			INTF_SETGET(Variant, rules)

			INTF_NULLABLE
			INTF_TO_JSONABLE
	};
}

MAKE_INSTANCEABLE(GameState)
