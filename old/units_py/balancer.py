from typing import List, Tuple, Optional
import random

from strategy import Strategy, StrategyGenerator, NullStrategy
from basic_strategy import BasicStrategy, basic_strategy_generator
from simulator import simulate
from resources import Resources
from unit_kind import unit_kinds


# Multi-armed bandit approach
def find_best_response(strategy: Strategy, generator: StrategyGenerator) -> Tuple[Strategy, float]:
    arms: List[Tuple[Strategy, int, int]] = []
    for i in range(5000):
        if len(arms) == 0 or random.random() < 0.5:
            candidate = generator()
            winners = simulate([strategy, candidate])
            if winners:
                arms.append((candidate, int(1 in winners), 1))
        else:
            # Choose the best strategy by percentage of wins
            _, i = max((wins / games, i) for (i, (_, wins, games)) in enumerate(arms))
            candidate, wins, games = arms[i]
            winners = simulate([strategy, candidate])
            if winners:
                arms[i] = (candidate, wins + int(1 in winners), games + 1)
    # Find the arm that has the highest win percentage among arms with at least 10 games
    _, i = max((wins / games, i) for (i, (_, wins, games)) in enumerate(arms) if games >= 10)
    candidate, wins, games = arms[i]
    return candidate, wins / games

# Local search approach

Trials = Tuple[int, int] # (wins, games)

# Compares two trials; returns 1 if the first is statistically better, -1 if
# the second is better, and 0 if the test is inconclusive.
def compare_trials(a: Trials, b: Trials) -> int:
    w1, g1 = a
    w2, g2 = b
    p1 = w1 / g1
    p2 = w2 / g2
    p = (g1 * p1 + g2 * p2) / (g1 + g2)
    if p <= 0 or p >= 1:
        return 0
    z = (p1 - p2) / (p * (1 - p) * (1 / g1 + 1 / g2))**0.5
    if abs(z) >= 1.96:
        return 1 if z > 0 else -1
    return 0

def search_for_best_response(strategy: Strategy, generator: StrategyGenerator) -> Tuple[Strategy, float]:
    best_strategy: Strategy = generator()
    best_wins = 0
    best_games = 0
    for i in range(10):
        current_strategy = generator()
        current_wins = 0
        current_games = 0
        mutations_without_improvement = 0
        while mutations_without_improvement < 25:
            # Make sure we have enough current games for statistical testing
            while current_games < 100:
                current_wins += int(1 in simulate([strategy, current_strategy]))
                current_games += 1
            # Generate a mutation
            candidate_strategy = current_strategy.mutate()
            candidate_wins = int(1 in simulate([strategy, candidate_strategy]))
            candidate_games = 1
            # Simulate games until we're sure which strategy is actually better
            while candidate_games < 100 and \
                  compare_trials((candidate_wins, candidate_games), (current_wins, current_games)) == 0:
                candidate_wins += int(1 in simulate([strategy, candidate_strategy]))
                candidate_games += 1
            # If the new strategy is better, replace the old strategy
            if compare_trials((candidate_wins, candidate_games), (current_wins, current_games)) > 0:
                current_strategy = candidate_strategy
                current_wins = candidate_wins
                current_games = candidate_games
                mutations_without_improvement = 0
            else:
                mutations_without_improvement += 1
        while current_games < 100:
            current_wins += int(1 in simulate([strategy, current_strategy]))
            current_games += 1
        # Determine if current_strategy is better than best_strategy
        if best_games == 0 or compare_trials((current_wins, current_games), (best_wins, best_games)) > 0:
            best_strategy = current_strategy
            best_wins = current_wins
            best_games = current_games
    return (best_strategy, best_wins / best_games)

if __name__ == '__main__':
    # current_best: Strategy = NullStrategy()
    current_best: Strategy = BasicStrategy(Resources(green=7), [unit_kinds['turret']] * 4)
    for i in range(10):
        new_strategy, win_percent = search_for_best_response(current_best, basic_strategy_generator)
        print(new_strategy, win_percent)
        if win_percent > 0.5:
            current_best = new_strategy
