from rules_schema import *

book = Book('contrition', 'Book of Contrition', [
  spell_tech(name='Learn Contrition', text='Increase Contrition by 1.'),

  Spell('1', 'Harm',
    'Lose 2 health. Deal 8 damage.',
    tech=1, level=1, cost=1,
    effects=[
      Effect(EK_health, ET_special, self=True, amount=-2),
      effect_attack(8),
    ],
  ),

  Spell('2', 'Repent',
    'Lose 7 health. Increase mana regeneration by 1.',
    tech=2, level=2, cost=2,
    effects=[
      Effect(EK_mana_regen, ET_special, self=True, amount=1),
      Effect(EK_health, ET_special, self=True, amount=-7),
    ],
  ),

  Spell('3', 'Steel Facade',
    'Decrease your mana regeneration by 1. Block 25 damage. Lasts two turns.',
    tech=3, level=3, cost=3,
    effects=[
      Effect(EK_mana_regen, ET_special, self=True, amount=-1),
      effect_shield(25, duration=duration_turns(2)),
    ],
  ),

  Spell('4', 'Fury',
    'Deal damage equal to the health you\'ve lost.',
    tech=4, level=4, cost=4,
    # Amount for this spell gets magically determined by the C++ code
    effects=[effect_attack(0)],
  ),
])
