import attr
import copy
from typing import List

from resources import Resources
from unit import Unit
from unit_kind import UnitKind, unit_kinds

@attr.s
class Player:
    name: str                         = attr.ib()
    units: List[Unit]                 = attr.ib()
    resources: Resources              = attr.ib()
    production: Resources             = attr.ib()
    done_turn: bool                   = attr.ib(default = False)
    alive: bool                       = attr.ib(default = True)
    build_order: List[List[UnitKind]] = attr.ib(factory = lambda: [])

    @staticmethod
    def create(name: str, production: Resources) -> 'Player':
        return Player(
                name = name,
                units = [Unit.of_kind(unit_kinds['general'])] + [Unit.of_kind(unit_kinds['wall']) for _ in range(5)],
                resources = Resources(),
                production = production)

    def clone(self) -> 'Player':
        p = Player(
                name = self.name,
                units = [Unit(health=u.health, kind=u.kind) for u in self.units],
                resources = copy.copy(self.resources),
                production = copy.copy(self.production),
                done_turn = self.done_turn,
                alive = self.alive)
        p.build_order = self.build_order[:]
        return p
