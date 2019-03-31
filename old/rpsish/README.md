# RPS-ish

Simple game for testing algorithms and languages.

## Rules

There are `N` different symbols and an `NxN` payoff table `A`, satisfying
`A[i][j] = -A[j][i]` and `A[i][i] = 0` (symbols tie against themselves and if X
beats Y, then Y loses to X).

A round consists of players simultaneously choosing two symbols, one public
(visible to the other player) and one private (not visible until the end of the
game, but not changeable either). There are K rounds. In the last round, only a
public symbol is chosen.

After all rounds are complete, compute the score for each player. If player one
has set of symbols `S` and player two has set of symbols `T`, then the payoff
for player one is given by the following pseudocode:

```python
score = 0
for s in S
  for t in T
    score += A[s][t]
```

Because of the properties of `A`, the game is zero sum. The player with
positive score wins.

## Summary of parameters

Implementations of the game should support arbitrary values for these
parameters.

- `N`: number of distinct symbols
- `A`: `NxN` integer payoff table for evaluating scores
- `K`: number of rounds in the game

## Justification for the game

The game is small enough that it can be solved by CFR, which means we can
compare our algorithms' results to what CFR tells us about the game.

The game is scalable so that we can test whether our algorithms still work when
the game is scaled up.
