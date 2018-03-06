"""The test game."""


PAYOFF_TABLE = [[2, 3],  # payoff_table[my_move][their_move]
                [-3, 1]
               ]
PLAYER_NUMBER = 2
ROUND_NUMBER = 5


class MoveInfo:
    """@brief A class containing all information about a player's turn.
    Used for information transfer and storage."""
    public_symbol = 0
    private_symbol = 0

    def __init__(self, public_symbol=0, private_symbol=0):
        self.public_symbol = public_symbol
        self.private_symbol = private_symbol


class ShroudedMoveInfo:
    """@brief A class containing all information about a player's turn, as visible to some player.
    Used for information transfer and storage.

    Note that this class contains all information that MoveInfo does, but everything starts as None - None elements
        represent elements that are shrouded from the intended recipient of this class."""
    public_symbol = None
    private_symbol = None


class TestPlayer:
    """@brief the player class."""

    # Each player precieves the game's history differently, and so must store their own perception locally.
    #     In this case, it is just a list of its own plays and the enemy's public symbols.
    enemy_moves = []
    my_moves = []

    def __init__(self):
        pass

    def generate_move(self):
        """@brief When this player must make a move, this function generates it."""
        # This logic should be more advanced.
        selected_move = MoveInfo(public_symbol=0, private_symbol=0)

        self.my_moves.append(selected_move)
        return selected_move

    def be_advised(self, enemy_move):
        """@brief After an enemy turn, this player is informed at least partially of the enemy's action."""
        self.enemy_moves.append(enemy_move)


class TestGame:
    """The class that contains the main game loop."""
    rounds = 0
    players = []  # Set of player classes
    moves = []  # Set of sets of moves, one set of moves per players

    def __init__(self, players, rounds=1):
        self.players = players
        self.rounds = rounds

        for _ in players:
            self.moves.append([])

    def game_loop(self):
        """@brief The game's main loop."""
        for round_number in range(0, self.rounds):
            player_number = round_number % len(self.players)

            players_move = self.players[player_number].generate_move()  # The player generates the next move.
            self.moves[player_number].append(players_move)  # The player's move is recorded into the game history

            # The player's move is processed to determine how every OTHER player views it, and then every other player
            #     is informed about the move. In this case, there is only one player, which always sees the previous
            #     move in exactly one way. However, with more players, this logic may get more complex.
            last_shroud = ShroudedMoveInfo()
            last_shroud.public_symbol = players_move.public_symbol

            # Inform all other players
            for other_player_number, player in enumerate(self.players):
                if other_player_number == player_number:
                    continue  # Don't advise ourselves

                player.be_advised(last_shroud)



        self.conclude()  # End game


    def conclude(self):
        """@brief Final wrap - calculate scores and declare a winner."""
        scores = []
        for _ in self.players:
            scores.append(0)

        for player_number, player_move_set in enumerate(self.moves):  # For each player...
            for move in player_move_set:  # For each move they made...
                # Calculate how much it adds to their score.
                public_symbol = move.public_symbol
                private_symbol = move.private_symbol

                for other_player_number, other_player_move_set in enumerate(self.moves):
                    if other_player_number == player_number:
                        continue  # Don't compare with our own moves

                    for enemy_move in other_player_move_set:
                        print("Comparing player {player_num}'s ({public_symbol}, {private_symbol}) against {enemy}'s ({enemy_public}, {enemy_private})...".format(player_num=player_number, public_symbol=public_symbol, private_symbol=private_symbol, enemy=other_player_number, enemy_public=enemy_move.public_symbol, enemy_private=enemy_move.private_symbol))
                        
                        add_score = PAYOFF_TABLE[public_symbol][enemy_move.public_symbol]
                        print("    ...pub-pub gets {add_score}...".format(add_score=add_score))
                        scores[player_number] += add_score

                        add_score = PAYOFF_TABLE[public_symbol][enemy_move.private_symbol]
                        print("    ...pub-priv gets {add_score}...".format(add_score=add_score))
                        scores[player_number] += add_score

                        add_score = PAYOFF_TABLE[private_symbol][enemy_move.public_symbol]
                        print("    ...priv-pub gets {add_score}...".format(add_score=add_score))
                        scores[player_number] += add_score

                        add_score = PAYOFF_TABLE[private_symbol][enemy_move.private_symbol]
                        print("    ...priv-priv gets {add_score}...".format(add_score=add_score))
                        scores[player_number] += add_score
        max_score = 0
        winner = 0
        for player_number, score in enumerate(scores):
            print("Player {player_number} got score {score}.".format(player_number=player_number, score=score))
            if score > max_score:
                max_score = score
                winner = player_number

        print("A WINNER IS |{winner}|!".format(winner=winner))


def main():
    players = []
    for _ in range(0, PLAYER_NUMBER):
        players.append(TestPlayer())

    game = TestGame(players=players, rounds=ROUND_NUMBER)
    game.game_loop()


if __name__ == '__main__':
    main()
