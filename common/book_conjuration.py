from rules_schema import *

book = Book('conjuration', 'Book of Conjuration', [
  spell_tech(name='Learn Conjuration', text='Increase Conjuration by 1.'),

  Spell('1', 'Heat Haze',
    'Block 13 damage from current spell only.',
    tech=1, level=1, cost=1,
    effects=[effect_shield(13, duration=duration_steps(1))],
  ),

  Spell('2', 'Burning Spirit',
    'Gain 3 mana next turn.',
    tech=2, level=2, cost=2,
    effects=[Effect(EK_mana, ET_special, self=True, amount=3)],
  ),

  Spell('3', 'Fireball',
    'Deal 13 damage.',
    tech=3, level=3, cost=3,
    effects=[effect_attack(13)],
  ),

  Spell('4', 'Incinerate',
    'Deal 10 piercing damage.',
    tech=4, level=4, cost=4,
    effects=[Effect(EK_health, ET_constant, amount=-10)],
  ),
])
