#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <strategy.h>
#include <random_strategy.h>
#include <cfr_strategy.h>

#include "macros.h"

namespace godot {
	class Strategy : public godot::GodotScript<Reference>, public Forwarder<::Strategy, Strategy> {
			GODOT_CLASS(Strategy)
		public:
			static const char *resource_path;
			static void _register_methods();

			void init_random_strategy();
			void init_cfr_strategy();
			Variant get_action(Variant game_view);

			INTF_NULLABLE
	};
}
