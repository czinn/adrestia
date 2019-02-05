#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <game_view.h>

#include "macros.h"

namespace godot {
	class GameView : public godot::GodotScript<Reference>, public Forwarder<::GameView, GameView> {
			GODOT_CLASS(GameView)
		public:
			static const char *resource_path;
			static void _register_methods();

			void init(Variant game_state, int view_player_id);
			void init_json(Variant rules, Variant json);
			Variant turn_number() const;
			Variant winners() const;
			INTF_SETGET(Variant, history)
			INTF_SETGET(Variant, players)
			INTF_SETGET(Variant, view_player_id)

			INTF_NULLABLE
			INTF_TO_JSONABLE
	};
}

MAKE_INSTANCEABLE(GameView)
