#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <sticky_instance.h>

#include "macros.h"

namespace godot {
	class EffectInstance;
	class Spell;
	class Duration;
	class GameRules;

	class StickyInstance : public godot::GodotScript<Reference>,
			public Forwarder<::StickyInstance, StickyInstance> {
			GODOT_CLASS(StickyInstance)
		private:
			godot::Ref<godot::NativeScript> Duration_;
			godot::Ref<godot::NativeScript> EffectInstance_;
			godot::Ref<godot::NativeScript> Spell_;
			godot::Ref<godot::NativeScript> Sticky_;
		public:
			StickyInstance();
			static const char *resource_path;
			static void _register_methods();

			void init(GameRules *rules, String json);
			Array apply_to_effect(int player_id, EffectInstance *effect);
			Array apply_to_spell(int player_id, Spell *spell);
			Array apply_to_turn(int player_id);

			INTF_SETGET(int, amount);
			INTF_SETGET(Variant, remaining_duration);
			INTF_SETGET(Variant, spell);
			INTF_SETGET(Variant, sticky);

			INTF_NULLABLE
			INTF_TO_JSONABLE
	};
}
