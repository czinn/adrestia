#include "game_rules.h"

#include "book.h"
#include "spell.h"
#include "sticky.h"

#define CLASSNAME GameRules

using namespace godot;

namespace godot {
	SCRIPT_AT("res://native/game_rules.gdns")

	void GameRules::_register_methods() {
		REGISTER_METHOD(get_version)
		REGISTER_METHOD(get_sticky)
		REGISTER_METHOD(get_spell)
		REGISTER_METHOD(get_book)
		REGISTER_METHOD(get_books)
		REGISTER_METHOD(get_mana_cap)
		REGISTER_METHOD(get_initial_health)
		REGISTER_METHOD(get_initial_mana_regen)
		REGISTER_NULLABLE
		REGISTER_JSONABLE
	}

	Variant CLASSNAME::get_version() const {
		json j = _ptr->get_version();
		return to_godot_variant(j, owner);
	}
	FORWARD_REF_BY_ID_GETTER(Sticky, get_sticky)
	FORWARD_REF_BY_ID_GETTER(Spell, get_spell)
	FORWARD_REF_BY_ID_GETTER(Book, get_book)
	FORWARD_AUTO_GETTER(get_books)
	FORWARD_AUTO_GETTER(get_mana_cap)
	FORWARD_AUTO_GETTER(get_initial_health)
	FORWARD_AUTO_GETTER(get_initial_mana_regen)

	IMPL_NULLABLE
	IMPL_JSONABLE
}
