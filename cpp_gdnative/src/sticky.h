#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <sticky.h>

#include "macros.h"

namespace godot {
	class Effect;
	class EffectInstance;
	class Spell;

	class Sticky : public godot::GodotScript<Reference>, public OwnerOrPointer<::Sticky> {
			GODOT_CLASS(Sticky)
		private:
			godot::Ref<godot::NativeScript> Effect_;
			godot::Ref<godot::NativeScript> EffectInstance_;
			godot::Ref<godot::NativeScript> Spell_;
		public:
			Sticky();
			static const char *resource_path;
			static void _register_methods();

			String get_id() const;
			String get_name() const;
			String get_text() const;
			int get_kind() const;
			Array get_effects() const;
			bool get_trigger_inbound() const;

			bool triggers_for_effect(EffectInstance *effect, bool inbound) const;
			bool triggers_for_spell(Spell *spell) const;
			bool triggers_at_end_of_turn() const;

			INTF_NULLABLE
			INTF_JSONABLE
	};
}
