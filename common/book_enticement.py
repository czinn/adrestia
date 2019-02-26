from rules_schema import *

book = Book('enticement', 'Book of Enticement', [
  spell_tech(name='Learn Enticement', text='Increase Enticement by 1.'),

  Spell('1', 'One',
    'Deal 3 damage. Block 3 damage from current spell only.',
    tech=1, level=1, cost=1,
    effects=[effect_attack(3), effect_shield(3, duration=duration_steps(1))],
  ),

  Spell('2', 'Two',
    'Deal 15 damage to shields.',
    tech=2, level=2, cost=2,
    effects=[Effect(EK_id, ET_attack, amount=15)],
  ),

  Spell('3', 'Three',
    'Deal 6 damage. On hit: gain 6 health',
    tech=3, level=3, cost=3,
    effects=[
      Effect(EK_health, ET_attack, amount=-6,
          on_hit=Effect(EK_health, ET_special, amount=6))
    ],
  ),

  Spell('4', 'Four',
    'Your opponent loses 1 mana regeneration.',
    tech=4, level=4, cost=4,
    effects=[Effect(EK_mana_regen, ET_special, amount=-1)],
  ),
])
