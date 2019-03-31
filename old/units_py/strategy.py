from abc import ABC, abstractmethod
from typing import Callable, List

from game_view import GameView
from unit_kind import UnitKind
from resources import Resources

class Strategy(ABC):
    # Gets the choice for the player's initial production. Total number of
    # resources should add up to 7.
    @abstractmethod
    def get_production(self) -> Resources:
        raise NotImplementedError

    # Returns a list of units to build. If the player does not have enough
    # resources to build a unit, it will be skipped, so there are no
    # requirements on what this method returns.
    @abstractmethod
    def do_turn(self, game_view: GameView) -> List[UnitKind]:
        raise NotImplementedError

    # Generates a new strategy based on the current strategy. Used for local
    # search.
    def mutate(self) -> 'Strategy':
        raise NotImplementedError

# A StrategyGenerator is a function that generates a random strategy.
StrategyGenerator = Callable[[], Strategy]

# Do-nothing strategy.
class NullStrategy(Strategy):
    def get_production(self) -> Resources:
        return Resources(red=3, green=2, blue=2)
    
    def do_turn(self, game_view: GameView) -> List[UnitKind]:
        return []

    def mutate(self) -> Strategy:
        return NullStrategy()
