#include "sticky_instance.h"

#include "duration.h"
#include "effect_instance.h"
#include "game_rules.h"
#include "spell.h"
#include "sticky.h"

#define CLASSNAME StickyInstance

using namespace godot;

namespace godot {
	const char *StickyInstance::resource_path = "res://native/sticky.gdns";

	StickyInstance::StickyInstance() {
		Duration_ = ResourceLoader::load(Duration::resource_path);
		EffectInstance_ = ResourceLoader::load(EffectInstance::resource_path);
		Spell_ = ResourceLoader::load(Spell::resource_path);
		Sticky_ = ResourceLoader::load(Sticky::resource_path);
	}

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

	Array StickyInstance::apply_to_effect(int player_id, EffectInstance *effect) {
		Array result;
		for (const auto &x : _ptr->apply(player_id, *effect->_ptr)) {
			auto [v, effect] = instance<EffectInstance>(EffectInstance_);
			effect->set_ptr(const_cast<::EffectInstance*>(&x), owner);
			result.append(v);
		}
		return result;
	}

	Array StickyInstance::apply_to_spell(int player_id, Spell *spell) {
		Array result;
		for (const auto &x : _ptr->apply(player_id, *spell->_ptr)) {
			auto [v, effect] = instance<EffectInstance>(EffectInstance_);
			effect->set_ptr(const_cast<::EffectInstance*>(&x), owner);
			result.append(v);
		}
		return result;
	}

	Array StickyInstance::apply_to_turn(int player_id) {
		Array result;
		for (const auto &x : _ptr->apply(player_id)) {
			auto [v, effect] = instance<EffectInstance>(EffectInstance_);
			effect->set_ptr(const_cast<::EffectInstance*>(&x), owner);
			result.append(v);
		}
		return result;
	}
	 
	IMPL_SETGET(int, amount)
	IMPL_SETGET_REF(Duration, remaining_duration)
	IMPL_SETGET_CONST_REF(Spell, spell)
	IMPL_SETGET_CONST_REF(Sticky, sticky)

	IMPL_NULLABLE
	IMPL_TO_JSONABLE
}
