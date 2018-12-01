#include "book.h"
#include "duration.h"
#include "effect.h"
#include "effect_instance.h"
#include "game_rules.h"
#include "game_state.h"
#include "game_view.h"
#include "gdexample.h"
#include "player.h"
#include "selector.h"
#include "spell.h"
#include "sticky.h"
#include "sticky_instance.h"
#include "sticky_invoker.h"
#include "strategy.h"

extern "C" void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *o) {
  godot::Godot::gdnative_init(o);
}

extern "C" void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *o) {
  godot::Godot::gdnative_terminate(o);
}

extern "C" void GDN_EXPORT godot_nativescript_init(void *handle) {
  godot::Godot::nativescript_init(handle);
  godot::register_class<godot::Book>();
  godot::register_class<godot::Duration>();
  godot::register_class<godot::Effect>();
  godot::register_class<godot::EffectInstance>();
  godot::register_class<godot::GameRules>();
  godot::register_class<godot::GameState>();
  godot::register_class<godot::GameView>();
  godot::register_class<godot::Player>();
  godot::register_class<godot::Selector>();
  godot::register_class<godot::Spell>();
  godot::register_class<godot::Sticky>();
  godot::register_class<godot::StickyInstance>();
  godot::register_class<godot::StickyInvoker>();
  godot::register_class<godot::Strategy>();
}
