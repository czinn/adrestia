import attr
from typing import List

from player import Player

@attr.s
class GameState:
    players: List[Player] = attr.ib()
    turn: int             = attr.ib()

    @staticmethod
    def create() -> 'GameState':
        return GameState(players=[], turn=0)

    def clone(self) -> 'GameState':
        return GameState(
                players=[p.clone() for p in self.players],
                turn=self.turn
            )
