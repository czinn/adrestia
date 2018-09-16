#include "game_view.h"
#include "action.h"
#include "colour.h"
#include "json.h"

GameView::GameView() {}

void to_json(json &j, const GameView &view) {
  j["view_index"] = view.view_index;
  j["view_player"] = view.view_player;
  j["players"] = view.players;
  j["turn"] = view.turn;
  j["action_log"] = view.action_log;
}

void GameView::generate_build_units_actions(std::vector<Action> &actions,
    std::vector<std::string> &units,
    int coins,
    int max_units,
    std::map<std::string, UnitKind>::const_iterator begin,
    std::map<std::string, UnitKind>::const_iterator end) const {
  if (begin == end || coins == 0 || max_units == 0) {
    actions.push_back(Action(units));
    return;
  }
  auto next_it = begin;
  next_it++;
  if (begin->second.get_tech() != nullptr &&
      view_player.tech.includes(*begin->second.get_tech()) &&
      begin->second.get_cost() <= coins) {
    units.push_back(begin->first);
    generate_build_units_actions(actions, units, coins - begin->second.get_cost(), max_units - 1, begin, end);
    units.pop_back();
  }
  generate_build_units_actions(actions, units, coins, max_units, next_it, end);
}

// Gets a list of legal actions for the player whose view this is.
// An empty list does not necessarily indicate that the game is over; it merely
// indicates that this player must wait for other players to act before
// continuing.
// All players should have no legal actions iff the game is over.
std::vector<Action> GameView::legal_actions() const {
  // Determine whether the player has selected their tech for this turn.
  int total_tech = view_player.tech.red + view_player.tech.green + view_player.tech.blue;
  if (total_tech < turn) {
    return std::vector<Action>({ RED, GREEN, BLUE });
  } else if (action_log[turn - 1][view_index].size() < 2) {
    std::vector<Action> actions;
    std::vector<std::string> units;
    generate_build_units_actions(actions, units, view_player.coins, rules->get_unit_cap() - view_player.units.size(),
        rules->get_unit_kinds().begin(), rules->get_unit_kinds().end());
    return actions;
  } else {
    return std::vector<Action>();
  }
}
