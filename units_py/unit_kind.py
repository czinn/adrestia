import attr
from typing import List, Optional, Callable, Dict, TYPE_CHECKING

from colour import Colour
from resources import Resources

if TYPE_CHECKING:
    from player import Player

@attr.s
class UnitKind:
    name: str                                         = attr.ib()
    colour: Colour                                    = attr.ib()
    health: int                                       = attr.ib()
    width: int                                        = attr.ib()
    attack: List[int]                                 = attr.ib()
    cost: Optional[Resources]                         = attr.ib()
    before_turn: Optional[Callable[['Player'], None]] = attr.ib(default = None)

def add_resources_effect(resources: Resources) -> Callable[['Player'], None]:
    def inner(p: 'Player') -> None:
        p.resources.add(resources)
    return inner

unit_kinds: Dict[str, UnitKind] = {
    'general': UnitKind(name='General', colour=Colour.BLACK, health=5, width=1, attack=[1], cost=None),
    'grunt': UnitKind(name='Grunt', colour=Colour.RED, health=1, width=1, attack=[1], cost=Resources(red=2)),
    'troll': UnitKind(name='Troll', colour=Colour.RED, health=2, width=2, attack=[4], cost=Resources(red=5)),
    'wall': UnitKind(name='Wall', colour=Colour.GREEN, health=1, width=1, attack=[], cost=Resources(green=1)),
    'turret': UnitKind(name='Turret', colour=Colour.GREEN, health=1, width=2, attack=[1], cost=Resources(green=2)),
    'wide_wall': UnitKind(name='Wide Wall', colour=Colour.GREEN, health=2, width=4, attack=[], cost=Resources(green=3)),
    'font_r': UnitKind(name='Font R', colour=Colour.BLUE, health=2, width=1, attack=[], cost=Resources(red=1, blue=2),
        before_turn=add_resources_effect(Resources(red=1))),
    'font_g': UnitKind(name='Font G', colour=Colour.BLUE, health=2, width=1, attack=[], cost=Resources(green=1, blue=2),
        before_turn=add_resources_effect(Resources(green=1))),
    'font_b': UnitKind(name='Font B', colour=Colour.BLUE, health=2, width=1, attack=[], cost=Resources(blue=3),
        before_turn=add_resources_effect(Resources(blue=1))),
    'paragon': UnitKind(name='Paragon', colour=Colour.BLUE, health=2, width=1, attack=[], cost=Resources(red=1, green=1, blue=7),
        before_turn=add_resources_effect(Resources(red=1, green=1, blue=1))),
    'ogre': UnitKind(name='Ogre', colour=Colour.RED, health=2, width=2, attack=[2], cost=Resources(red=2, green=2)),
    'shrek': UnitKind(name='Shrek', colour=Colour.GREEN, health=4, width=3, attack=[2], cost=Resources(red=2, green=3)),
    'soldier': UnitKind(name='Soldier', colour=Colour.BLUE, health=1, width=1, attack=[1], cost=Resources(red=1, blue=2)),
    'commando': UnitKind(name='Commando', colour=Colour.RED, health=2, width=1, attack=[1, 1], cost=Resources(red=3, blue=2)),
    # TODO: forts shouldn't take damage from red units
    'fort': UnitKind(name='Fort', colour=Colour.GREEN, health=1, width=4, attack=[], cost=Resources(green=3, blue=2)),
    'insurgent': UnitKind(name='Insurgent', colour=Colour.RED, health=3, width=1, attack=[1], cost=Resources(red=2, green=1, blue=1)),
    'avatar': UnitKind(name='Avatar', colour=Colour.BLUE, health=3, width=2, attack=[1, 3, 1], cost=Resources(red=5, green=1, blue=3)),
    'tank': UnitKind(name='Tank', colour=Colour.BLUE, health=10, width=7, attack=[2], cost=Resources(red=1, green=5, blue=3)),
}