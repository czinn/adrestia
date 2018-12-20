#include "sticky.h"

#include "effect.h"
#include "effect_instance.h"
#include "spell.h"

#define CLASSNAME Sticky

using namespace godot;

namespace godot {
	SCRIPT_AT("res://native/sticky.gdns")

	void Sticky::_register_methods() {
		REGISTER_METHOD(get_id)
		REGISTER_METHOD(get_name)
		REGISTER_METHOD(get_text)
		REGISTER_METHOD(get_kind)
		REGISTER_METHOD(get_stacks)
		REGISTER_METHOD(get_effects)
		REGISTER_METHOD(get_trigger_inbound)
		REGISTER_METHOD(triggers_for_effect)
		REGISTER_METHOD(triggers_for_spell)
		REGISTER_METHOD(triggers_at_end_of_turn)
		REGISTER_NULLABLE
		REGISTER_JSONABLE
	}

	FORWARD_AUTO_GETTER(get_id)
	FORWARD_AUTO_GETTER(get_name)
	FORWARD_AUTO_GETTER(get_text)
	FORWARD_AUTO_GETTER(get_kind)
	FORWARD_AUTO_GETTER(get_stacks)
	FORWARD_AUTO_GETTER(get_effects)
	FORWARD_AUTO_GETTER(get_trigger_inbound)
	bool Sticky::triggers_for_effect(EffectInstance *effect, bool inbound) const {
		return _ptr->triggers_for_effect(*effect->_ptr, inbound);
	}
	bool Sticky::triggers_for_spell(Spell *spell) const {
		return _ptr->triggers_for_spell(*spell->_ptr);
	}
	FORWARD_AUTO_GETTER(triggers_at_end_of_turn)

	IMPL_NULLABLE
	IMPL_JSONABLE
}
