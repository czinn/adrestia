#include "game_view.h"

#include "game_state.h"
#include "player.h"

#define CLASSNAME GameView

using namespace godot;

namespace godot {
	SCRIPT_AT("res://native/game_view.gdns")

	void CLASSNAME::_register_methods() {
		REGISTER_METHOD(init)
		REGISTER_METHOD(init_json)
		REGISTER_METHOD(turn_number)
		REGISTER_METHOD(winners)
		REGISTER_SETGET(history, Variant())
		REGISTER_SETGET(players, Variant())
		REGISTER_SETGET(view_player_id, Variant())
		REGISTER_NULLABLE
		REGISTER_TO_JSONABLE
	}

	void CLASSNAME::init(Variant game_state, int view_player_id) {
		auto *_state = godot::as<GameState>(game_state);
		set_ptr(new ::GameView(*_state->_ptr, view_player_id));
	}

	void CLASSNAME::init_json(Variant rules, Variant json) {
		nlohmann::json j;
		auto *_rules = godot::as<GameRules>(rules);
		of_godot_variant(json, &j);
		set_ptr(new ::GameView(*_rules->_ptr, j));
	}

	FORWARD_AUTO_GETTER(turn_number)
	FORWARD_AUTO_GETTER(winners)

	IMPL_SETGET_CONST_AUTO(history)
	IMPL_SETGET_CONST_AUTO(players)
	IMPL_SETGET_CONST_AUTO(view_player_id)

	IMPL_NULLABLE
	IMPL_TO_JSONABLE
}

