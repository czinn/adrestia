#include "spell.h"

#include "effect.h"
#include "selector.h"

#define CLASSNAME Spell

using namespace godot;

namespace godot {
	SCRIPT_AT("res://native/spell.gdns")

	void Spell::_register_methods() {
		REGISTER_METHOD(get_id)
		REGISTER_METHOD(get_name)
		REGISTER_METHOD(get_book)
		REGISTER_METHOD(get_tech)
		REGISTER_METHOD(get_level)
		REGISTER_METHOD(get_cost)
		REGISTER_METHOD(get_text)
		REGISTER_METHOD(get_effects)
		REGISTER_METHOD(is_counterspell)
		REGISTER_METHOD(is_tech_spell)
		REGISTER_METHOD(get_counterspell_selector)
		REGISTER_NULLABLE
		REGISTER_JSONABLE
	}

	FORWARD_AUTO_GETTER(get_id)
	FORWARD_AUTO_GETTER(get_name)
	FORWARD_AUTO_GETTER(get_book)
	FORWARD_AUTO_GETTER(get_tech)
	FORWARD_AUTO_GETTER(get_level)
	FORWARD_AUTO_GETTER(get_cost)
	FORWARD_AUTO_GETTER(get_text)
	FORWARD_AUTO_GETTER(get_effects)
	FORWARD_AUTO_GETTER(is_counterspell)
	FORWARD_AUTO_GETTER(is_tech_spell)
	FORWARD_AUTO_GETTER(get_counterspell_selector)

	IMPL_NULLABLE
	IMPL_JSONABLE
}
