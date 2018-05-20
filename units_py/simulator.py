import itertools
from typing import List, Optional
import random
import os

from colour import Colour
from resources import Resources
from unit_kind import UnitKind, unit_kinds
from unit import Unit
from player import Player
from game_state import GameState
from game_view import GameView, OtherPlayer
from strategy import Strategy

def simulate(strategies: List[Strategy], debug: bool = False) -> List[int]:
    state: GameState = GameState.create()
    names: List[str] = ['Alice', 'Bob', 'Carol']
    if debug:
        print('Starting game.')
    for name, strategy in zip(names, strategies):
        state.players.append(Player.create(name, strategy.get_production()))

    # Main game loop
    while True:
        # Production
        for player in state.players:
            player.resources.add(player.production)

            # Pre-turn effects
            for unit in player.units:
                if unit.kind.before_turn is not None:
                    unit.kind.before_turn(player)

        # Build
        for player_index, player in enumerate(state.players):
            strategy = strategies[player_index]
            build_list = strategy.do_turn(GameView.of_gamestate(state, player_index))
            for kind in build_list:
                if kind.cost is not None and player.resources.subsumes(kind.cost):
                    player.resources.subtract(kind.cost)
                    player.units.append(Unit.of_kind(kind))

        # Battle
        if debug:
            os.system('clear')
        for player in state.players:
            target_units = list(itertools.chain.from_iterable(p.units for p in state.players if p is not player))
            total_width = sum(u.kind.width for u in target_units)
            for attacker in player.units:
                for attack in attacker.kind.attack:
                    width_left = random.randint(0, total_width - 1)
                    target: Optional[Unit] = None
                    for u in target_units:
                        width_left -= u.kind.width
                        if width_left < 0:
                            target = u
                            break
                    if target is None:
                        raise Exception('Invalid target during battle')
                    target.health -= attack
                    if debug:
                        message = f"{player.name}'s {attacker.kind.name} hits {target.kind.name} for {attack}"
                        if target.health <= 0 and target.health + attack > 0:
                            message += ", killing it!"
                        print(message)
        if debug:
            print('\nPress enter to continue')
            input()

        # Death
        for player in state.players:
            player.units = [u for u in player.units if u.health > 0]
            player.alive = any(u.kind is unit_kinds['general'] for u in player.units)
        
        winners = list(enumerate(p for p in state.players if p.alive))
        if debug:
            if len(winners) > 1:
                print('Winners: {", ".join(p.name for _, p in winners)}')
            elif len(winners) == 0:
                print(f'{winners[0][1].name} wins!')
            else:
                print('Everyone loses!')
        return [i for i, _ in winners]
        if debug:
            print()
