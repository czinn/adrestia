#include "gdexample.h"
#include "action.h"
#include "battle.h"
#include "colour.h"
#include "game_rules.h"
#include "game_state.h"
#include "game_view.h"
#include "player.h"
#include "player_view.h"
#include "tech.h"
#include "unit.h"
#include "unit_kind.h"

extern "C" void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *o) {
  godot::Godot::gdnative_init(o);
}

extern "C" void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *o) {
  godot::Godot::gdnative_terminate(o);
}

extern "C" void GDN_EXPORT godot_nativescript_init(void *handle) {
  godot::Godot::nativescript_init(handle);
  godot::register_class<godot::gdexample>();
  godot::register_class<godot::Action>();
  godot::register_class<godot::Battle>();
  godot::register_class<godot::Colour>();
  godot::register_class<godot::GameRules>();
  godot::register_class<godot::GameState>();
  godot::register_class<godot::GameView>();
  godot::register_class<godot::Player>();
  godot::register_class<godot::PlayerView>();
  godot::register_class<godot::Tech>();
  godot::register_class<godot::Unit>();
  godot::register_class<godot::UnitKind>();
}
