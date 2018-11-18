#include "gdexample.h"
#include "book.h"
#include "duration.h"
#include "sticky_invoker.h"
#include "effect.h"
#include "spell.h"
#include "selector.h"
#include "effect_instance.h"
#include "sticky.h"
#include "game_rules.h"

extern "C" void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *o) {
  godot::Godot::gdnative_init(o);
}

extern "C" void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *o) {
  godot::Godot::gdnative_terminate(o);
}

extern "C" void GDN_EXPORT godot_nativescript_init(void *handle) {
  godot::Godot::nativescript_init(handle);
  godot::register_class<godot::gdexample>();
  godot::register_class<godot::Book>();
  godot::register_class<godot::Duration>();
  godot::register_class<godot::StickyInvoker>();
  godot::register_class<godot::Effect>();
  godot::register_class<godot::Spell>();
  godot::register_class<godot::Selector>();
  godot::register_class<godot::EffectInstance>();
  godot::register_class<godot::Sticky>();
  godot::register_class<godot::GameRules>();
}
