import attr
import copy
import random
from typing import List

from game_view import GameView
from resources import Resources
from strategy import Strategy
from unit_kind import UnitKind, unit_kinds

def random_kind() -> UnitKind:
    while True:
        kind = random.choice(list(unit_kinds.values()))
        if kind.cost:
            return kind

@attr.s
class BasicStrategy(Strategy):
    production: Resources = attr.ib()
    build_order: List[UnitKind] = attr.ib()
    
    def get_production(self) -> Resources:
        return self.production

    def do_turn(self, view: GameView) -> List[UnitKind]:
        resources = copy.copy(view.view_player.resources)
        build = []
        for kind in self.build_order:
            if kind.cost is None:
                continue
            if resources.subsumes(kind.cost):
                build.append(kind)
                resources.subtract(kind.cost)
        return build

    def mutate(self) -> Strategy:
        if random.random() < 0.5:
            # Insert a new unit
            i = random.randint(0, len(self.build_order))
            return BasicStrategy(self.production,
                                 self.build_order[:i] + [random_kind()] + self.build_order[i:])
        else:
            # Remove a unit
            i = random.randint(0, len(self.build_order) - 1)
            return BasicStrategy(self.production,
                                 self.build_order[:i] + self.build_order[i+1:])

    def __str__(self) -> str:
        unit_list = '[{}]'.format(','.join(k.name for k in self.build_order))
        return f'<BasicStrategy {self.production} {unit_list}>'

def basic_strategy_generator() -> Strategy:
    build_order = [random_kind()]
    while random.random() > 0.5:
        build_order.append(random_kind())
    a, b = random.randint(0, 7), random.randint(0, 7)
    red = min(a, b)
    green = abs(a - b)
    blue = 7 - red - green
    return BasicStrategy(production=Resources(red=red, green=green, blue=blue),
                         build_order=build_order)
