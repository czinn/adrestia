#include "game_state.h"

using namespace godot;

#define CLASSNAME GameState

namespace godot {
  const char *GameState::resource_path = "res://native/game_state.gdns";

  GameState::GameState() {
    Action_ = ResourceLoader::load(Action::resource_path);
    Battle_ = ResourceLoader::load(Battle::resource_path);
    GameRules_ = ResourceLoader::load(GameRules::resource_path);
    Player_ = ResourceLoader::load(Tech::resource_path);
  }

  void GameState::_register_methods() {
    REGISTER_METHOD(init)
    REGISTER_METHOD(perform_action)
    REGISTER_METHOD(get_rules)
    REGISTER_METHOD(get_turn)
    REGISTER_METHOD(get_players)
    REGISTER_METHOD(get_action_log)
    REGISTER_METHOD(get_battles)
    REGISTER_NULLABLE
    REGISTER_TO_JSONABLE
  }

  void GameState::init(GameRules *rules, int num_players) {
    set_ptr(new ::GameState(*rules->_ptr, num_players));
  }

  bool GameState::perform_action(int pid, Action *action) {
    return _ptr->perform_action(pid, *action->_ptr);
  }

  FORWARD_REF_GETTER(GameRules, get_rules)
  FORWARD_GETTER(int, get_turn);
  FORWARD_REF_ARRAY_GETTER(Player, get_players)

  Array CLASSNAME::get_action_log() const {
    Array result;
    for (const auto &x1 : _ptr->get_action_log()) {
      Array r1;
      for (const auto &x2 : x1) {
        Array r2;
        for (const auto &x : x2) {
          auto [v, thing] = instance<Action>(Action_);
          thing->set_ptr(const_cast<::Action*>(&x), owner);
          r2.append(v);
        }
        r1.append(r2);
      }
      result.append(r1);
    }
    return result;
  }

  Array CLASSNAME::get_battles() const {
    Array result;
    for (const auto &b : _ptr->get_battles()) {
      auto [v, thing] = instance<Battle>(Battle_);
      thing->set_ptr(const_cast<::Battle*>(b.get()), owner);
      result.append(v);
    }
    return result;
  }

  IMPL_NULLABLE
  IMPL_TO_JSONABLE
}
