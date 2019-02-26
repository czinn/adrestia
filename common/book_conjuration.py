from rules_schema import *

book = Book('conjuration', 'Book of Conjuration', [
  spell_tech(name='Learn Conjuration', text='Increase Conjuration by 1.'),

  Spell('attack_1', 'Flame Strike',
    'Deal 3 damage.',
    tech=1, level=1, cost=1,
    effects=[Effect(EK_health, ET_attack, self=False, amount=-3,
        on_hit=Effect(EK_health, ET_special, self=True, amount=-1))],
  ),

  Spell('shield_1', 'Fortress',
    'Block 8 damage. Lasts two turns.',
    tech=2, level=2, cost=3,
    effects=[effect_shield(8, duration_turns(2))],
  ),

  Spell('attack_2', 'Lifesteal',
    'Deal 8 damage. Gain 4 health.',
    tech=3, level=3, cost=4,
    effects=[
        effect_attack(8),
        Effect(EK_health, ET_special, self=True, amount=4),
    ]
  ),

  Spell('attack_3', 'Erupt',
    'Deal 16 damage.',
    tech=4, level=4, cost=5,
    effects=[effect_attack(16)],
  ),
])
