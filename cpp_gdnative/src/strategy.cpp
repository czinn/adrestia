#include "strategy.h"

#include "game_action.h"
#include "game_view.h"

#define CLASSNAME Strategy

using namespace godot;

namespace godot {
	SCRIPT_AT("res://native/strategy.gdns")

	void CLASSNAME::_register_methods() {
		REGISTER_METHOD(init_random_strategy)
		REGISTER_METHOD(get_action)
		REGISTER_NULLABLE
	}

	void CLASSNAME::init_random_strategy() {
		set_ptr(new ::RandomStrategy());
	}

	Variant CLASSNAME::get_action(Variant view) {
		auto *_view = godot::as<GameView>(view);
		return to_godot_variant(_ptr->get_action(*_view->_ptr), owner);
	}

	IMPL_NULLABLE
}
