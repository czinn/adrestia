#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <selector.h>

#include "macros.h"

namespace godot {
	class Spell;
	class EffectInstance;

	class Selector : public Reference, public Forwarder<::Selector, Selector> {
			GODOT_CLASS(Selector, Reference)
		public:
			static const char *resource_path;
			static void _register_methods();

			Variant is_valid();
			bool selects_spell(Spell *spell);
			bool selects_effect(EffectInstance *effect);

			INTF_NULLABLE
			INTF_JSONABLE
	};
}

MAKE_INSTANCEABLE(Selector)
