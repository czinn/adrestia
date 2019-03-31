from abc import ABC, abstractmethod
import attr
import copy
import math
import random
import time
from typing import List, Optional, Tuple

from game_view import GameView, OtherPlayer
from game_state import GameState
from resources import Resources
from strategy import Strategy
from unit_kind import UnitKind, unit_kinds
from simulator import simulate_turn

# r is a tuple of (min_resources, observed_resources), where
# min_resources is the minimum allocation to each colour necessary to be able to afford the observed units, and
# observed_resources is how much the player has been observed to spend, less any resources produced by fonts
def update_observed_resources(r: Tuple[Resources, Resources], p: OtherPlayer, turn: int) -> Tuple[Resources, Resources]:
    min_resources, observed_resources = r
    if len(p.build_order) > 0:
        for k in p.build_order[-1]:
            if k.cost is not None:
                observed_resources.add(k.cost)
    if turn > 0:
        min_resources.red = max(math.ceil(observed_resources.red / turn), min_resources.red)
        min_resources.green = max(math.ceil(observed_resources.green / turn), min_resources.green)
        min_resources.blue = max(math.ceil(observed_resources.blue / turn), min_resources.blue)
    for u in p.units:
        if u.kind.font:
            observed_resources.subtract(u.kind.font)
    return (min_resources, observed_resources)

# List of possible build orders given the available resources
def possible_build_orders(resources: Resources) -> List[List[UnitKind]]:
    build_orders = []
    unit_kind_list = [k for k in unit_kinds.values() if k.cost is not None]
    def aux(build_order: List[UnitKind], r: Resources, unit_kind_index: int) -> None:
        if unit_kind_index >= len(unit_kind_list):
            build_orders.append(build_order)
            return
        kind = unit_kind_list[unit_kind_index]
        if kind.cost is not None and r.subsumes(kind.cost):
            nr = Resources(r.red, r.green, r.blue)
            nr.subtract(kind.cost)
            aux(build_order + [kind], nr, unit_kind_index)
        aux(build_order, r, unit_kind_index + 1)
    aux([], resources, 0)
    return build_orders

class RandomStrategy(Strategy):
    def get_production(self) -> Resources:
        return Resources(3, 2, 2)

    def do_turn(self, view: GameView) -> List[UnitKind]:
        r = view.view_player.resources
        build = []
        while random.random() < 0.99 and r.red + r.green + r.blue > 0:
            kind = random.choice(list(unit_kinds.values()))
            if kind.cost is not None and r.subsumes(kind.cost):
                build.append(kind)
                r.subtract(kind.cost)
        return build

# Returns the winner. -1 if its a tie or the turn limit is reached.
# Turn limit is from the current turn, not the game's turn
def playout(state: GameState, turn_limit: int) -> int:
    for i in range(turn_limit):
        simulate_turn(state, [RandomStrategy() for p in state.players], debug=False)
        winners = [i for i, p in enumerate(state.players) if p.alive]
        if len(winners) == 1:
            return winners[0]
        elif len(winners) == 0:
            return -1
    return -2

class PartialStrategy(ABC):
    @abstractmethod
    def get_production(self) -> Optional[Resources]:
        raise NotImplementedError

    @abstractmethod
    def do_turn(self, game_view: GameView) -> Optional[List[UnitKind]]:
        raise NotImplementedError

@attr.s
class FixedStrategy(Strategy):
    build: List[UnitKind] = attr.ib()

    def get_production(self) -> Resources:
        return Resources(3, 2, 2)

    def do_turn(self, view: GameView) -> List[UnitKind]:
        return self.build

class MonteStrategy(Strategy):
    partial_strategy: PartialStrategy
    observed_resources: Optional[List[Tuple[Resources, Resources]]]
    turn: int

    def __init__(self, partial_strategy: PartialStrategy) -> None:
        self.partial_strategy = partial_strategy
        self.observed_resources = None
        self.turn = 0

    def get_production(self) -> Resources:
        p = self.partial_strategy.get_production()
        if p is not None:
            return p
        # TODO: charles: determine resources to use by simulation if not
        # specified by the partial strategy
        return Resources(3, 2, 2)

    def do_turn(self, view: GameView) -> List[UnitKind]:
        t = self.partial_strategy.do_turn(view)
        if t is not None:
            self.turn += 1
            return t

        # Find current resources for other players
        current_resources = []
        for i, p in enumerate(view.other_players):
            r = Resources()
            for build in p.build_order:
                r.add(view.productions[i])
                for u in build:
                    if u.cost is not None:
                        r.subtract(u.cost)
            r.add(view.productions[i])
            current_resources.append(r)

        # Determine all possible build orders for this turn
        possible_orders = [possible_build_orders(view.view_player.resources)] + [possible_build_orders(r) for r in current_resources]

        # Record of plays and wins for each player; wins[i][j]/plays[i][j] is
        # the fraction of times that player i has played strategy j that resulted
        # in a win for player i
        wins = [[0 for _ in o] for o in possible_orders]
        plays = [[0 for _ in o] for o in possible_orders]

        # Create game state from view based, for running simulations
        state = view.to_gamestate(current_resources)
        start_time = time.time()
        c = 2**0.5
        while time.time() < start_time + 5.0:
            selected_orders = []
            for j in range(len(wins)):
                N = sum(plays[j])
                if not any(x == 0 for x in plays[j]):
                    order = max((wins[j][i] / plays[j][i] + c * (math.log(N) / plays[j][i]), i) for i in range(len(wins[j])))[1]
                else:
                    order = random.choice([i for i in range(len(plays[j])) if plays[j][i] == 0])
                plays[j][order] += 1
                selected_orders.append(order)
            copy = state.clone()
            simulate_turn(copy, [FixedStrategy(po[i]) for po, i in zip(possible_orders, selected_orders)], debug=False)
            result = playout(copy, 10)
            if result > -1:
                wins[result][selected_orders[result]] += 1

        # Find order for self with highest fraction of wins
        chosen_order = max((wins[0][i] / plays[0][i], i) for i in range(len(wins[0])) if plays[0][i] > 0)[1]
        return possible_orders[0][chosen_order]
