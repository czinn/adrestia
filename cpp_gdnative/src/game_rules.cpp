#include "game_rules.h"

#include "book.h"
#include "spell.h"
#include "sticky.h"

#define CLASSNAME GameRules

using namespace godot;

namespace godot {
	const char *GameRules::resource_path = "res://native/game_rules.gdns";

	GameRules::GameRules() {
		Book_ = ResourceLoader::load(Book::resource_path);
		Spell_ = ResourceLoader::load(Spell::resource_path);
		Sticky_ = ResourceLoader::load(Sticky::resource_path);
	}

	void GameRules::_register_methods() {
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

	FORWARD_REF_BY_ID_GETTER(Sticky, get_sticky)
	FORWARD_REF_BY_ID_GETTER(Spell, get_spell)
	FORWARD_REF_BY_ID_GETTER(Book, get_book)
  Dictionary GameRules::get_books() {
    Dictionary result;
    for (auto &[book_id, book] : _ptr->get_books()) {
      auto [v, b] = instance<Book>(Book_);
      b->set_ptr(const_cast<::Book*>(&book), owner);
      result[String(book_id.c_str())] = v;
    }
    return result;
  }
	FORWARD_GETTER(int, get_mana_cap)
	FORWARD_GETTER(int, get_initial_health)
	FORWARD_GETTER(int, get_initial_mana_regen)

	IMPL_NULLABLE
	IMPL_JSONABLE
}
