#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <effect_instance.h>

#include "macros.h"

namespace godot {
	class GameRules;
	class Player;

	class EffectInstance : public godot::GodotScript<Reference>, public Forwarder<::EffectInstance, EffectInstance> {
			GODOT_CLASS(EffectInstance)
		private:
			godot::Ref<godot::NativeScript> Spell_;
			godot::Ref<godot::NativeScript> StickyInvoker_;
		public:
			EffectInstance();
			static const char *resource_path;
			static void _register_methods();

			void apply(GameRules *rules, Player *player) const;

			INTF_SETGET(int, kind)
			INTF_SETGET(bool, targets_self)
			INTF_SETGET(int, effect_type)
			INTF_SETGET(int, amount)
			INTF_SETGET(Variant, sticky_invoker)
			INTF_SETGET(Variant, spell)
			INTF_SETGET(int, target_player)

			INTF_NULLABLE
			INTF_TO_JSONABLE
	};
}
