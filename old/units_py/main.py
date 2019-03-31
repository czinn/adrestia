import attr
from typing import Optional, List

from resources import Resources
from strategy import Strategy
from human_strategy import HumanStrategy
from monte_strategy import MonteStrategy, PartialStrategy
from simulator import simulate
from game_view import GameView
from unit_kind import UnitKind

################################################################################
# Text-based manual playtesting system
################################################################################

@attr.s
class OnlyStartPartialStrategy(PartialStrategy):
    resources: Resources = attr.ib()

    def get_production(self) -> Optional[Resources]:
        return self.resources
    
    def do_turn(self, game_view: GameView) -> Optional[List[UnitKind]]:
        return None

if __name__ == '__main__':
    strategies: List[Strategy] = [
            MonteStrategy(partial_strategy=OnlyStartPartialStrategy(resources=Resources(4, 2, 1))),
            MonteStrategy(partial_strategy=OnlyStartPartialStrategy(resources=Resources(2, 4, 1)))
        ]
    simulate(strategies, debug=True)
