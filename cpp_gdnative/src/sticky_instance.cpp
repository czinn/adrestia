#include "sticky_instance.h"

#include "duration.h"
#include "effect_instance.h"
#include "game_rules.h"
#include "spell.h"
#include "sticky.h"

#define CLASSNAME StickyInstance

using namespace godot;

namespace godot {
	SCRIPT_AT("res://native/sticky_instance.gdns");

	void StickyInstance::_register_methods() {
		REGISTER_METHOD(apply_to_effect)
		REGISTER_METHOD(apply_to_spell)
		REGISTER_METHOD(apply_to_turn)
		REGISTER_SETGET(amount, 0)
		REGISTER_SETGET(remaining_duration, Variant())
		REGISTER_SETGET(spell, Variant())
		REGISTER_SETGET(sticky, Variant())
		REGISTER_NULLABLE
		REGISTER_TO_JSONABLE
	}

	void StickyInstance::init(GameRules *rules, String json) {
		set_ptr(new ::StickyInstance(*rules->_ptr, nlohmann::json::parse(json.utf8().get_data())));
	}

	Variant StickyInstance::apply_to_effect(int player_id, EffectInstance *effect) {
		return to_godot_variant(_ptr->apply_to_effect(player_id, *effect->_ptr), this);
	}

	Variant StickyInstance::apply_to_spell(int player_id, Spell *spell) {
		return to_godot_variant(_ptr->apply_to_spell(player_id, *spell->_ptr), this);
	}

	Variant StickyInstance::apply_to_turn(int player_id) {
		return to_godot_variant(_ptr->apply_to_turn(player_id), this);
	}

	IMPL_SETGET(int, amount)
	IMPL_SETGET_CONST_AUTO(remaining_duration)
	IMPL_SETGET_CONST_AUTO(spell)
	IMPL_SETGET_CONST_AUTO(sticky)

	IMPL_NULLABLE
	IMPL_TO_JSONABLE
}
