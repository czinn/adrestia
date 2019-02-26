from rules_schema import *

book = Book('bloodlust', 'Book of Bloodlust', [
  spell_tech(name='Learn Bloodlust', text='Increase Bloodlust by 1.'),

  Spell('attack_1', 'Razor Wind',
    'Deal 5 damage.',
    tech=1, level=1, cost=2,
    effects=[effect_attack(5)],
  ),

  Spell('attack_2', 'Gathering Storm',
    "Deal 3 damage, plus 1 for each time you previously cast this spell.",
    tech=2, level=2, cost=2,
    effects=[
      effect_attack(3),
      Effect(EK_sticky, ET_special, self=True, sticky=StickyInvoker('storm', duration_game, -1)),
    ],
  ),

  Spell('special', 'Bloodlust',
    'All spells you cast deal +2 damage for the rest of the game.',
    tech=3, level=3, cost=5,
    effects=[
      Effect(EK_sticky, ET_special, self=True,
        sticky=StickyInvoker('bloodlust', duration_game, amount=-2)),
    ]
  ),

  Spell('attack_3', 'Piercing Shot',
    "Deal 7 unblockable damage.",
    tech=4, level=4, cost=4,
    effects=[Effect(EK_health, ET_constant, amount=-7)],
  ),
])
