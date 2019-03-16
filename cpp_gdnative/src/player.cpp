#include "player.h"

#include "book.h"
#include "effect_instance.h"
#include "game_rules.h"
#include "spell.h"
#include "sticky_instance.h"

#define CLASSNAME Player

using namespace godot;

namespace godot {
	SCRIPT_AT("res://native/player.gdns")

	void Player::_register_methods() {
		REGISTER_METHOD(find_book_idx);
		REGISTER_METHOD(level);
		REGISTER_METHOD(pipe_effect);
		REGISTER_METHOD(pipe_spell);
		REGISTER_METHOD(pipe_turn);
		REGISTER_METHOD(subtract_step);
		REGISTER_METHOD(subtract_turn);
		REGISTER_SETGET(id, -1);
		REGISTER_SETGET(hp, -1);
		REGISTER_SETGET(max_hp, -1);
		REGISTER_SETGET(mp, -1);
		REGISTER_SETGET(mp_regen, -1);
		REGISTER_SETGET(tech, Variant())
		REGISTER_SETGET(books, Variant())
		REGISTER_SETGET(stickies, Variant())
		REGISTER_NULLABLE
		REGISTER_TO_JSONABLE
	}

	int Player::find_book_idx(String book_id) {
		return _ptr->find_book_idx(std::string(book_id.ascii().get_data()));
	}

	FORWARD_AUTO_GETTER(level)

	Variant Player::pipe_effect(EffectInstance *effect, bool inbound) {
		return to_godot_variant(_ptr->pipe_effect(*effect->_ptr, inbound), this);
	}

	Variant Player::pipe_spell(Spell *spell) {
		return to_godot_variant(_ptr->pipe_spell(*spell->_ptr), this);
	}

	Variant Player::pipe_turn() {
		return to_godot_variant(_ptr->pipe_turn(), this);
	}

	FORWARD_VOID(subtract_step)
	FORWARD_VOID(subtract_turn)

	IMPL_SETGET(int, id)
	IMPL_SETGET(int, hp)
	IMPL_SETGET(int, max_hp)
	IMPL_SETGET(int, mp)
	IMPL_SETGET(int, mp_regen)
	IMPL_SETGET_CONST_AUTO(tech)
	IMPL_SETGET_CONST_AUTO(books)
	IMPL_SETGET_CONST_AUTO(stickies)

	IMPL_NULLABLE
	IMPL_TO_JSONABLE
}
