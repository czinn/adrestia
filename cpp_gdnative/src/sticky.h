#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <sticky.h>

#include "macros.h"

namespace godot {
	class Effect;
	class EffectInstance;
	class Spell;

	class Sticky : public Reference, public Forwarder<::Sticky, Sticky> {
			GODOT_CLASS(Sticky, Reference)
		public:
			static const char *resource_path;
			static void _register_methods();

			Variant get_id();
			Variant get_name();
			Variant get_text();
			Variant get_kind();
			Variant get_stacks();
			Variant get_effects();
			Variant get_trigger_inbound();
			bool triggers_for_effect(EffectInstance *effect, bool inbound);
			bool triggers_for_spell(Spell *spell);
			Variant triggers_at_end_of_turn();

			INTF_NULLABLE
			INTF_JSONABLE
	};
}

MAKE_INSTANCEABLE(Sticky)
