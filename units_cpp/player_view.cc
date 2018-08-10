#include "player_view.h"
#include "player.h"

PlayerView::PlayerView() {}

PlayerView::PlayerView(const Player &player)
    : units(player.units)
    , alive(player.alive)
    , coins(player.coins) {}

PlayerView::PlayerView(const GameRules &rules, const json &j) : alive(j["alive"]) {
  for (auto it = j["units"].begin(); it != j["units"].end(); it++) {
    const UnitKind &kind = rules.get_unit_kind(it.value()["kind"]);
    units.emplace(std::stoi(it.key()), Unit(kind, it.value()));
  }
}

void to_json(json &j, const PlayerView &view) {
  for (auto &&[unit_id, unit] : view.units) {
    j["units"][std::to_string(unit_id)] = unit;
  }
  j["alive"] = view.alive;
}
