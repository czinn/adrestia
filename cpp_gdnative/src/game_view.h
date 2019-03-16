#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <game_view.h>

#include "macros.h"

namespace godot {
	class GameState;
	class GameRules;

	class GameView : public Reference, public Forwarder<::GameView, GameView> {
			GODOT_CLASS(GameView, Reference)
		public:
			static const char *resource_path;
			static void _register_methods();

			void init(GameState *game_state, int view_player_id);
			void init_json(GameRules *rules, Variant json);
			Variant turn_number();
			Variant winners();
			INTF_SETGET(Variant, history)
			INTF_SETGET(Variant, players)
			INTF_SETGET(Variant, view_player_id)

			INTF_NULLABLE
			INTF_TO_JSONABLE
	};
}

MAKE_INSTANCEABLE(GameView)
