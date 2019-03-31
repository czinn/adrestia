import attr
import copy
from typing import List

from game_state import GameState
from player import Player
from resources import Resources
from unit import Unit
from unit_kind import UnitKind

@attr.s
class OtherPlayer:
    name: str                         = attr.ib()
    units: List[Unit]                 = attr.ib()
    build_order: List[List[UnitKind]] = attr.ib()
    alive: bool                       = attr.ib()

    @staticmethod
    def of_player(player: Player) -> 'OtherPlayer':
        return OtherPlayer(name=player.name, units=player.units[:], build_order=player.build_order[:], alive=player.alive)

@attr.s
class GameView:
    view_player: Player              = attr.ib()
    other_players: List[OtherPlayer] = attr.ib()
    turn: int                        = attr.ib()
    # Reading productions is technically cheating, but we allow it because it
    # makes the AI simpler.
    productions: List[Resources]     = attr.ib()

    # Returns a player's view of the game state (all the information they can
    # see). Modifying this view should not affect the original game state in
    # any way.
    @staticmethod
    def of_gamestate(game: GameState, player_index: int) -> 'GameView':
        return GameView(view_player=game.players[player_index].clone(),
                        other_players=[OtherPlayer.of_player(p) for i, p in enumerate(game.players) if i != player_index],
                        turn=game.turn,
                        productions=[p.production for i, p in enumerate(game.players) if i != player_index])

    def to_gamestate(self, resources: List[Resources]) -> GameState:
        return GameState(players=[self.view_player] + [
                Player(
                    p.name,
                    p.units,
                    resources[i],
                    self.productions[i],
                    alive=p.alive
                )
                for i, p in enumerate(self.other_players)
            ], turn=self.turn)
