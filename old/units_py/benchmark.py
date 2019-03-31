# Crude benchmark for python3 vs pypy3.
import time

from strategy import Strategy
from basic_strategy import BasicStrategy
from simulator import simulate
from resources import Resources
from unit_kind import unit_kinds

strategy1 = BasicStrategy(Resources(green=7), [unit_kinds['turret']] * 4)
strategy2 = BasicStrategy(Resources(red=4, green=3), [unit_kinds[x] for x in 'shrek troll grunt commando'.split()])

print(f'Strategy 1: {strategy1}')
print(f'Strategy 2: {strategy2}')

start_time = time.time()
total = 10000
wins = 0
for _ in range(total):
    wins += int(0 in simulate([strategy1, strategy2]))
elapsed_time = time.time() - start_time
print(f'Strategy 1 win rate: {wins / total}')
print(f'Elapsed time for {total} simulations: {elapsed_time}')
