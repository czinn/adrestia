#include "player.h"
#include <algorithm>
#include "unit_kind.h"
#include "game_rules.h"
#include "json.h"

Player::Player() : alive(true), coins(0), next_unit(0) {}

Player::Player(const GameRules &rules) : alive(true), coins(0), next_unit(0) {
  std::vector<UnitKind*> starting_units = rules.get_starting_units();
  for (auto it = starting_units.begin(); it != starting_units.end(); it++) {
    build_unit(**it);
  }
}

Player::Player(const Player &player)
    : units(player.units)
    , alive(player.alive)
    , coins(player.coins)
    , tech(player.tech)
    , next_unit(player.next_unit) {}

Player::Player(const GameRules &rules, const json &j)
    : alive(j["alive"])
    , coins(j["coins"])
    , tech(j["tech"])
    , next_unit(j["next_unit"]) {
  for (auto it = j["units"].begin(); it != j["units"].end(); it++) {
    const UnitKind &kind = rules.get_unit_kind(it.value()["kind"]);
    units.emplace(std::stoi(it.key()), Unit(kind, it.value()));
  }
}

Player &Player::operator=(Player &player) {
  std::swap(units, player.units);
  std::swap(alive, player.alive);
  std::swap(coins, player.coins);
  std::swap(tech, player.tech);
  std::swap(next_unit, player.next_unit);
  return *this;
}

void Player::build_unit(const UnitKind &kind) {
  units.emplace(next_unit, Unit(kind));
  next_unit++;
}

void Player::begin_turn() {
  for (auto it = units.begin(); it != units.end(); it++) {
    if (it->second.build_time > 0) {
      it->second.build_time -= 1;
    }
    coins += it->second.kind.get_font();
  }
}

void to_json(json &j, const Player &player) {
  for (auto &&[unit_id, unit] : player.units) {
    j["units"][std::to_string(unit_id)] = unit;
  }
  j["alive"] = player.alive;
  j["coins"] = player.coins;
  j["tech"] = player.tech;
  j["next_unit"] = player.next_unit;
}
