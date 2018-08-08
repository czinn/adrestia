#pragma once
#include <vector>
#include <memory>
#include <random>
#include "game_rules.h"
#include "game_view.h"
#include "action.h"
#include "player.h"
#include "player_view.h"
#include "battle.h"
#include "tech.h"
#include "json.h"

using json = nlohmann::json;

class GameState {
 public:
  GameState(const GameRules &rules, int num_players);
  GameState(const GameRules &rules, const json &j);
  GameState(const GameState &game_state);
  GameState(const GameView &view, const std::vector<Tech> &techs); // Determinization with the given hidden information
  bool perform_action(int player, const Action &action);
  void get_view(GameView &view, int player) const;
  std::vector<int> get_winners() const; // Empty list indicates that game is still in progress
  const GameRules &get_rules() const;
  int get_turn() const;
  const std::vector<Player> &get_players() const;
  const std::vector<std::vector<std::vector<Action>>> &get_action_log() const;
  const std::vector<std::shared_ptr<Battle>> &get_battles() const;

  friend void to_json(json &j, const GameState &game_state);
 private:
  const GameRules &rules;
  std::vector<Player> players;
  std::vector<std::vector<std::vector<Action>>> action_log;
  std::vector<std::shared_ptr<Battle>> battles;
  int turn;
  int players_ready;

  void begin_turn();
  void execute_battle(); // Executes the battle and advances the turn
};
