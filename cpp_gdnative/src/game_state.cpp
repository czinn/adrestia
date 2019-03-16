#include "game_state.h"

#include "player.h"
#include "game_rules.h"
#include "game_view.h"

#define CLASSNAME GameState

using namespace godot;

namespace godot {
	SCRIPT_AT("res://native/game_state.gdns")

	void CLASSNAME::_register_methods() {
		REGISTER_METHOD(init)
		REGISTER_METHOD(init_json)
		REGISTER_METHOD(clone)
		REGISTER_METHOD(of_game_view)
		REGISTER_METHOD(simulate)
		REGISTER_METHOD(simulate_events)
		REGISTER_METHOD(apply_event)
		REGISTER_METHOD(is_valid_action)
		REGISTER_METHOD(turn_number)
		REGISTER_METHOD(winners)
		REGISTER_SETGET(history, Variant())
		REGISTER_SETGET(players, Variant())
		REGISTER_SETGET(rules, Variant())
		REGISTER_NULLABLE
		REGISTER_TO_JSONABLE
	}

	void CLASSNAME::init(godot::GameRules *rules, Variant player_books) {
		Array a = player_books;
		std::vector<std::vector<std::string>> _books;
		of_godot_variant(player_books, &_books);

		// XTODO: jim: Is it possible that the memory underlying the rules will be
		// freed by Godot? Should we hold a reference to rules somehow to prevent
		// that?
		// jim: answer: yes it is fucking important, thanks past jim. done now
		// TODO: this is still a bit flaky, so let's keep all rules alive forever
		// anyway. take a look at this later
		set_ptr(new ::GameState(*rules->_ptr, _books));
		_deps.push_back(godot::Ref<godot::Reference>(rules));
	}

	void CLASSNAME::init_json(GameRules *rules, Variant json) {
		nlohmann::json j;
		of_godot_variant(json, &j);
		set_ptr(new ::GameState(*rules->_ptr, j));
		_deps.push_back(godot::Ref<godot::Reference>(rules));
	}

	void CLASSNAME::clone(GameState *state) {
		set_ptr(new ::GameState(*state->_ptr));
	}

	void CLASSNAME::of_game_view(GameView *view) {
		::GameView &v = *view->_ptr;
		size_t i = v.view_player_id;
		set_ptr(new ::GameState(v, v.players[i].tech, v.players[i].books));
	}

	bool CLASSNAME::is_valid_action(int player_id, Variant action) {
		::GameAction action_;
		of_godot_variant(action, &action_);
		return _ptr->is_valid_action(player_id, action_);
	}

	bool CLASSNAME::simulate(Variant actions) {
		std::vector<::GameAction> actions_;
		of_godot_variant(actions, &actions_);
		return _ptr->simulate(actions_);
	}

	Variant CLASSNAME::simulate_events(Variant actions) {
		std::vector<::GameAction> actions_;
		std::vector<nlohmann::json> events_out;
		of_godot_variant(actions, &actions_);
		_ptr->simulate(actions_, events_out);
		return to_godot_variant(events_out, this);
	}

	void CLASSNAME::apply_event(Variant event) {
		nlohmann::json j;
		of_godot_variant(event, &j);
		_ptr->apply_event(j);
	}

	FORWARD_AUTO_GETTER(turn_number)
	FORWARD_AUTO_GETTER(winners)

	IMPL_SETGET_CONST_AUTO(history)
	IMPL_SETGET_CONST_AUTO(players)
	IMPL_SETGET_CONST_AUTO(rules)

	IMPL_NULLABLE
	IMPL_TO_JSONABLE
}
