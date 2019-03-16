#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <game_state.h>
#include <game_action.h>

#include "macros.h"

namespace godot {
	class GameRules;
	class GameView;
	class GameState : public Reference, public Forwarder<::GameState, GameState> {
			GODOT_CLASS(GameState, Reference)
		public:
			static const char *resource_path;
			static void _register_methods();

			void init(GameRules *rules, Variant player_books);
			void init_json(GameRules *rules, Variant json);
			void clone(GameState *state);
			// Returns a game state not suitable for simulation, but suitable for apply_event
			void of_game_view(GameView *view);
			bool simulate(Variant actions);
			Variant simulate_events(Variant actions);
			void apply_event(Variant event);

			bool is_valid_action(int player_id, Variant action);
			Variant turn_number();
			Variant winners();

			INTF_SETGET(Variant, history)
			INTF_SETGET(Variant, players)
			INTF_SETGET(Variant, rules)

			INTF_NULLABLE
			INTF_TO_JSONABLE
	};
}

MAKE_INSTANCEABLE(GameState)
