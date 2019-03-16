#pragma once
#include <Godot.hpp>
#include <Reference.hpp>
#include <strategy.h>
#include <random_strategy.h>
#include <cfr_strategy.h>

#include "macros.h"

namespace godot {
	class GameRules;
	class GameView;
	class Strategy : public Reference, public Forwarder<::Strategy, Strategy> {
			GODOT_CLASS(Strategy, Reference)
		public:
			static const char *resource_path;
			static void _register_methods();

			void init_random_strategy();
			void init_cfr_strategy(GameRules *rules);
			Variant get_action(GameView *game_view);

			INTF_NULLABLE
	};
}
