#include "player.h"

#include "book.h"
#include "effect_instance.h"
#include "game_rules.h"
#include "spell.h"
#include "sticky_instance.h"

#define CLASSNAME Player

using namespace godot;

namespace godot {
	const char *Player::resource_path = "res://native/player.gdns";

	Player::Player() {
		Book_ = ResourceLoader::load(Book::resource_path);
		EffectInstance_ = ResourceLoader::load(EffectInstance::resource_path);
		GameRules_ = ResourceLoader::load(GameRules::resource_path);
		Spell_ = ResourceLoader::load(Spell::resource_path);
		StickyInstance_ = ResourceLoader::load(StickyInstance::resource_path);
	}

	void Player::_register_methods() {
		REGISTER_METHOD(find_book_idx);
		REGISTER_METHOD(level);
		REGISTER_METHOD(pipe_effect);
		REGISTER_METHOD(pipe_spell);
		REGISTER_METHOD(pipe_turn);
		REGISTER_METHOD(subtract_step);
		REGISTER_METHOD(subtract_turn);
		REGISTER_SETGET(hp, -1);
		REGISTER_SETGET(max_hp, -1);
		REGISTER_SETGET(mp, -1);
		REGISTER_SETGET(mp_regen, -1);
		REGISTER_SETGET(tech, Array())
		REGISTER_SETGET(books, Array())
		REGISTER_SETGET(stickies, Array())
		REGISTER_NULLABLE
		REGISTER_TO_JSONABLE
	}

	int Player::find_book_idx(String book_id) const {
		return _ptr->find_book_idx(std::string(book_id.ascii().get_data()));
	}

	FORWARD_GETTER(int, level)

	// TODO: charles: These functions and the corresponding ones in
	// StickyInstance seem like they could be generalized. Maybe do that.
	Array Player::pipe_effect(int player_id, EffectInstance *effect, bool inbound) {
		Array result;
		for (const auto &x : _ptr->pipe_effect(player_id, *effect->_ptr, inbound)) {
			auto [v, effect] = instance<EffectInstance>(EffectInstance_);
			effect->set_ptr(const_cast<::EffectInstance*>(&x), owner);
			result.append(v);
		}
		return result;
	}

	Array Player::pipe_spell(int player_id, Spell *spell) {
		Array result;
		for (const auto &x : _ptr->pipe_spell(player_id, *spell->_ptr)) {
			auto [v, effect] = instance<EffectInstance>(EffectInstance_);
			effect->set_ptr(const_cast<::EffectInstance*>(&x), owner);
			result.append(v);
		}
		return result;
	}

	Array Player::pipe_turn(int player_id) {
		Array result;
		for (const auto &x : _ptr->pipe_turn(player_id)) {
			auto [v, effect] = instance<EffectInstance>(EffectInstance_);
			effect->set_ptr(const_cast<::EffectInstance*>(&x), owner);
			result.append(v);
		}
		return result;
	}

	FORWARD_VOID(subtract_step)
	FORWARD_VOID(subtract_turn)

	IMPL_SETGET(int, hp)
	IMPL_SETGET(int, max_hp)
	IMPL_SETGET(int, mp)
	IMPL_SETGET(int, mp_regen)
	IMPL_SETGET_ARRAY(tech)

	Array Player::get_books() const {
		Array result;
		for (const auto &x : _ptr->books) {
			auto [v, thing] = instance<Book>(Book_);
			thing->set_ptr(const_cast<::Book*>(x), owner);
			result.append(v);
		}
		return result;
	}
	void CLASSNAME::set_books(Array v) {
		Godot::print("Error: Called Player::set_books (setter for an array member)");
		assert(false);
	}

	IMPL_SETGET_REF_ARRAY(StickyInstance, stickies)

	IMPL_NULLABLE
	IMPL_TO_JSONABLE
}
