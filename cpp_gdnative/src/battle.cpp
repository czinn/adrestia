#include "battle.h"

using namespace godot;


namespace godot {

#define CLASSNAME Attack
  const char *Attack::resource_path = "res://native/attack.gdns";

  void Attack::_register_methods() {
    REGISTER_NULLABLE;
    REGISTER_SETGET(from_player, 0);
    REGISTER_SETGET(from_unit, 0);
    REGISTER_SETGET(to_player, 0);
    REGISTER_SETGET(to_unit, 0);
    REGISTER_SETGET(damage, 0);
  }

  IMPL_SETGET(int, from_player);
  IMPL_SETGET(int, from_unit);
  IMPL_SETGET(int, to_player);
  IMPL_SETGET(int, to_unit);
  IMPL_SETGET(int, damage);

  IMPL_NULLABLE

#undef CLASSNAME
#define CLASSNAME Battle
  const char *Battle::resource_path = "res://native/battle.gdns";

  Battle::Battle() {
    Attack_ = ResourceLoader::load(Attack::resource_path);
    PlayerView_ = ResourceLoader::load(PlayerView::resource_path);
  }

  void Battle::_register_methods() {
    REGISTER_METHOD(get_players);
    REGISTER_METHOD(get_players_after);
    REGISTER_METHOD(get_attacks);
    REGISTER_NULLABLE;
    REGISTER_TO_JSONABLE;
  }

  FORWARD_REF_ARRAY_GETTER(PlayerView, get_players)
  FORWARD_REF_ARRAY_GETTER(PlayerView, get_players_after)
  FORWARD_REF_ARRAY_GETTER(Attack, get_attacks)

  IMPL_NULLABLE
  IMPL_TO_JSONABLE
}
