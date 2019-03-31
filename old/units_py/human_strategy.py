import os
from typing import List

from game_view import GameView
from resources import Resources
from strategy import Strategy
from unit_kind import UnitKind, unit_kinds

# Requests actions from standard input with human-readable prompts and input formats.
class HumanStrategy(Strategy):
    def get_production(self) -> Resources:
        os.system('clear')
        while True:
            print('Select production (R G B): ', end='')
            production: Resources = Resources.of_string_exn(input())
            if production.red + production.green + production.blue != 7:
                print('Must sum to 7')
                continue
            return production

    def do_turn(self, view: GameView) -> List[UnitKind]:
        build_list: List[UnitKind] = []

        player = view.view_player

        os.system('clear')
        print('\nPress enter to start turn')
        input()
        print('Player: {}'.format(player.name))
        for other in view.other_players:
            print('{}\'s units:\n'.format(other.name) + '\n'.join(u.to_string_hum() for u in other.units))
            print()
        while True:
            print('Resources available: {}'.format(player.resources.to_string_hum()))
            print('Your units:\n' + '\n'.join(u.to_string_hum() for u in player.units))
            print()
            print('Building this turn: ' + ' '.join(u.name for u in build_list))
            print('Type a unit name to buy it, or "done".')
            print('Available units: ' + ' '.join(unit_kinds.keys()))
            cmd = input()
            if cmd == 'done': break
            unit_kind = unit_kinds.get(cmd)
            if unit_kind is None:
                print('Invalid unit')
                continue
            if unit_kind.cost is None:
                print('Cannot buy.')
                continue
            if not player.resources.subsumes(unit_kind.cost):
                print('Cannot afford.')
                continue
            player.resources.subtract(unit_kind.cost)
            build_list.append(unit_kind)

        return build_list
