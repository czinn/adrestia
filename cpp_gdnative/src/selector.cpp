#include "selector.h"

#include "spell.h"
#include "effect_instance.h"

#define CLASSNAME Selector

using namespace godot;

namespace godot {
	SCRIPT_AT("res://native/selector.gdns")

	Selector::Selector() {
		Spell_ = ResourceLoader::load(Spell::resource_path);
		EffectInstance_ = ResourceLoader::load(EffectInstance::resource_path);
	}

	void Selector::_register_methods() {
		REGISTER_METHOD(is_valid);
		REGISTER_METHOD(selects_spell);
		REGISTER_METHOD(selects_effect);
		REGISTER_NULLABLE
		REGISTER_JSONABLE
	}

	FORWARD_AUTO_GETTER(is_valid);

	bool Selector::selects_spell(Spell *spell) const {
		return _ptr->selects_spell(*spell->_ptr);
	}

	bool Selector::selects_effect(EffectInstance *effect) const {
		return _ptr->selects_effect(*effect->_ptr);
	}

	IMPL_NULLABLE
	IMPL_JSONABLE
}
