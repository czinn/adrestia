#include "game_state.h"

#include "player.h"
#include "game_rules.h"

#define CLASSNAME GameState

using namespace godot;

namespace godot {
	SCRIPT_AT("res://native/game_state.gdns")

	void CLASSNAME::_register_methods() {
		REGISTER_METHOD(init)
		REGISTER_METHOD(is_valid_action)
		REGISTER_METHOD(simulate)
		REGISTER_METHOD(turn_number)
		REGISTER_METHOD(winners)
		REGISTER_SETGET(history, Variant())
		REGISTER_SETGET(players, Variant())
		REGISTER_SETGET(rules, Variant())
		REGISTER_NULLABLE
		REGISTER_TO_JSONABLE
	}

	void CLASSNAME::init(Variant rules, Variant player_books) {
		Array a = player_books;
		auto *_rules = godot::as<GameRules>(rules);
		std::vector<std::vector<std::string>> _books;
		of_godot_variant(player_books, &_books);

		// TODO: jim: Is it possible that the memory underlying the rules will be
		// freed by Godot? Should we hold a reference to rules somehow to prevent
		// that?
		set_ptr(new ::GameState(*_rules->_ptr, _books));
	}

	bool CLASSNAME::is_valid_action(int player_id, Variant action) const {
		::GameAction action_;
		of_godot_variant(action, &action_);
		return _ptr->is_valid_action(player_id, action_);
	}

	bool CLASSNAME::simulate(Variant actions) {
		std::vector<::GameAction> actions_;
		of_godot_variant(actions, &actions_);
		return _ptr->simulate(actions_);
	}

	FORWARD_AUTO_GETTER(turn_number)
	FORWARD_AUTO_GETTER(winners)

	IMPL_SETGET_CONST_AUTO(history)
	IMPL_SETGET_CONST_AUTO(players)
	IMPL_SETGET_CONST_AUTO(rules)

	IMPL_NULLABLE
	IMPL_TO_JSONABLE
}