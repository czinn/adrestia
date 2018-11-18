#include "effect_instance.h"

#include "spell.h"
#include "sticky_invoker.h"

#define CLASSNAME EffectInstance

using namespace godot;

namespace godot {
	const char *EffectInstance::resource_path = "res://native/effect_instance.gdns";

	EffectInstance::EffectInstance() {
		Spell_ = ResourceLoader::load(Spell::resource_path);
		StickyInvoker_ = ResourceLoader::load(StickyInvoker::resource_path);
	}

	void EffectInstance::_register_methods() {
		REGISTER_SETGET(kind, -1)
		REGISTER_SETGET(targets_self, false)
		REGISTER_SETGET(effect_type, -1)
		REGISTER_SETGET(amount, 0)
		REGISTER_SETGET(sticky_invoker, Variant())
		REGISTER_SETGET(spell, Variant())
		REGISTER_SETGET(target_player, -1)

		REGISTER_NULLABLE
		REGISTER_TO_JSONABLE
	}

	IMPL_SETGET_ENUM(EffectKind, kind)
	IMPL_SETGET(bool, targets_self)
	IMPL_SETGET_ENUM(EffectType, effect_type)
	IMPL_SETGET(int, amount)
  IMPL_SETGET_CONST_REF(StickyInvoker, sticky_invoker)
  IMPL_SETGET_CONST_REF(Spell, spell)
	IMPL_SETGET(int, target_player)

	IMPL_NULLABLE
	IMPL_TO_JSONABLE
}
