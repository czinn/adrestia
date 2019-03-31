import * as rl from 'readline-sync';
const readline = require('readline')

enum Colour {
  Black,
  Red,
  Green,
  Blue,
}

namespace Resources {
  export interface t {
    red: number,
    green: number,
    blue: number,
  }
  export const empty = { red: 0, green: 0, blue: 0 };
  export function of_string_exn(s: string): t {
    const [red, green, blue] = s.split(' ').map(x => parseInt(x));
    return { red, green, blue };
  }
  export function to_string_hum(t1: t): string {
    return `((red ${t1.red}) (green ${t1.green}) (blue ${t1.blue}))`;
  }
  export function subsumes(t1: t, t2: t): boolean {
    return t1.red <= t2.red && t1.green <= t2.green && t1.blue <= t2.blue;
  }
  export function add(t1: t, t2: t): t {
    return {
      red: t1.red + t2.red,
      green: t1.green + t2.green,
      blue: t1.blue + t2.blue,
    };
  }
  export function subtract(t1: t, t2: t): t {
    return {
      red: t1.red - t2.red,
      green: t1.green - t2.green,
      blue: t1.blue - t2.blue,
    };
  }
}

namespace UnitKind {
  export interface t {
    name: string,
    colour: Colour,
    health: number,
    width: number,
    attack: number[],
    cost: Resources.t | null,
    before_turn: ((player: Player.t) => void) | null,
  }

  export const general: t = {
    name: 'General',
    colour: Colour.Black,
    health: 5,
    width: 1,
    attack: [1],
    cost: null,
    before_turn: null,
  }

  export const grunt: t = {
    name: 'Grunt',
    colour: Colour.Red,
    health: 1,
    width: 1,
    attack: [1],
    cost: { red: 2, green: 0, blue: 0 },
    before_turn: null,
  }

  export const font_r: t = {
    name: 'Font R',
    colour: Colour.Red,
    health: 2,
    width: 1,
    attack: [],
    cost: { red: 1, green: 0, blue: 2 },
    before_turn: player => { player.resources.red += 1; },
  }

  export const font_g: t = {
    name: 'Font G',
    colour: Colour.Green,
    health: 2,
    width: 1,
    attack: [],
    cost: { red: 0, green: 1, blue: 2 },
    before_turn: player => { player.resources.green += 1; },
  }

  export const font_b: t = {
    name: 'Font B',
    colour: Colour.Blue,
    health: 2,
    width: 1,
    attack: [],
    cost: { red: 0, green: 0, blue: 3 },
    before_turn: player => { player.resources.blue += 1; },
  }
}

const UnitKinds: { [key: string]: UnitKind.t | undefined } = {
  general: UnitKind.general,
  grunt: UnitKind.grunt,
  font_r: UnitKind.font_r,
  font_g: UnitKind.font_g,
  font_b: UnitKind.font_b,
};

namespace Unit {
  export interface t {
    kind: UnitKind.t,
    hp: number,
  }

  export function create(kind: UnitKind.t): t {
    return {
      kind,
      hp: kind.health,
    }
  }

  export function to_string_hum(unit: t): string {
    return (
      `(${unit.kind.name}` +
      ` (width ${unit.kind.width})` +
      ` (hp ${unit.hp}/${unit.kind.health})` +
      (unit.kind.attack.length ? ` (attack ${unit.kind.attack})` : '') +
      ')'
    );
  }
}

namespace Player {
  export interface t {
    name: string,
    units: Unit.t[],
    resources: Resources.t,
    production: Resources.t,
    done_turn: boolean,
    alive: boolean,
  }
  export function create(name: string, production: Resources.t): t {
    return {
      name,
      units: [Unit.create(UnitKind.general)],
      resources: Resources.empty,
      production,
      done_turn: false,
      alive: true,
    };
  }
}

namespace GameState {
  export interface t {
    players: Player.t[],
  }
  export const empty: t = {
    players: [],
  };
}

function sum(xs: number[]): number {
  return xs.reduce((a, b) => a + b);
}

// lower inclusive, upper exclusive
function rand_int(lower: number, upper: number): number {
  return Math.floor(Math.random() * (upper - lower)) + lower;
}

let state = GameState.empty;
console.log('Starting game.');
function read_production(player_name: string): Resources.t {
  while (true) {
    const production =
      Resources.of_string_exn(
        rl.question(`${player_name}'s production (R G B): `));
    if (production.red + production.blue + production.green !== 7) {
      console.log('Must sum to 7');
      continue;
    }
    return production;
  }
}

['Alice', 'Bob'].forEach(name => {
  state.players.push(Player.create(name, read_production(name)));
});

while (true) {
  // Production
  state.players.forEach(player => {
    player.resources = Resources.add(player.resources, player.production);
    // Pre-turn effects
    player.units.forEach(unit => {
      if (unit.kind.before_turn !== null) {
        unit.kind.before_turn(player);
      }
    });
  });

  // Build
  for (let pid = 0; pid < 2; pid += 1) {
    console.log(`Player ${pid+1}`);
    const player = state.players[pid];
    while (true) {
      console.log(`Resources available: ${Resources.to_string_hum(player.resources)}`);
      console.log(`Your units:\n${player.units.map(u => Unit.to_string_hum(u)).join('\n')}`);
      console.log('Type a unit name to buy it, or "done".');
      console.log(`Available units: ${Object.keys(UnitKinds).sort().join()}`);
      const cmd = rl.question('> ');
      if (cmd === 'done') break;
      const unit_kind = UnitKinds[cmd];
      if (unit_kind === undefined) { console.log('Invalid unit.'); continue; }
      if (unit_kind.cost === null) { console.log('Cannot buy.'); continue; }
      if (!Resources.subsumes(unit_kind.cost, player.resources)) {
        console.log('Cannot afford.');
        continue;
      }
      player.resources = Resources.subtract(player.resources, unit_kind.cost);
      player.units.push(Unit.create(unit_kind));
    }
  }

  // Battle
  for (let attacker_id = 0; attacker_id < 2; attacker_id += 1) {
    const defender_id = (attacker_id + 1) % 2;
    const attacker = state.players[attacker_id];
    const defender = state.players[defender_id];
    const total_width = sum(defender.units.map(u => u.kind.width));
    attacker.units.forEach(u => {
      u.kind.attack.forEach(atk => {
        let width_left = rand_int(0, total_width);
        let target: Unit.t | undefined;
        for (const du of defender.units) {
          width_left -= du.kind.width;
          if (width_left < 0) { target = du; break; }
        }
        if (target === undefined) { throw 'Invalid target!' }
        target.hp -= atk;
        console.log(
          `${attacker.name}'s ${u.kind.name}` +
          ` attacks ${defender.name}'s ${target.kind.name}` +
          ` for ${atk}.`);
      });
    });
  }

  // Reaping
  for (let pid = 0; pid < 2; pid += 1) {
    const player = state.players[pid];
    let player_lost = false;
    player.units = player.units.filter(u => {
      if (u.hp > 0) { return true; }
      if (u.kind === UnitKind.general) { player_lost = true; }
      console.log(`${player.name}'s ${u.kind.name} died!`);
      return false;
    });
    if (player_lost) {
      console.log(`${player.name} is so ded!`);
      player.alive = false;
    }
  }

  // Check for win
  const remaining_players = state.players.filter(p => p.alive);
  if (remaining_players.length < 2) {
    if (remaining_players.length === 1) {
      console.log(`${remaining_players[0].name} wins!`);
    } else {
      console.log('It is a tie!');
    }
    break;
  }
}

console.log('Play again some time!');
