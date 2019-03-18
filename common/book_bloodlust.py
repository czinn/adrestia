from rules_schema import *

book = Book('bloodlust', 'Book of Bloodlust', [
  spell_tech(name='Learn Bloodlust', text='Increase Bloodlust by 1.'),

  Spell('1', 'Razor Wind',
    'Deal 5 damage.',
    tech=1, level=1, cost=1,
    effects=[effect_attack(5)],
  ),

  Spell('2', 'Frenzy',
    'Deal 3 damage. Your attacks deal +1 damage for the rest of the game.',
    tech=2, level=2, cost=2,
    effects=[
      effect_attack(3),
      Effect(EK_sticky, ET_special, self=True,
          sticky=StickyInvoker('bloodlust', duration_game, amount=-1)),
    ],
  ),

  Spell('3', 'Bloodboil',
    'Deal 9 damage. On hit: gain 1 mana.',
    tech=3, level=3, cost=3,
    effects=[
      Effect(EK_health, ET_attack, amount=-9,
          on_hit=Effect(EK_mana, ET_special, amount=1))
    ],
  ),

  Spell('4', 'Mangle',
    'Deal 16 damage. On hit: opponent loses 3 health per turn for 4 turns.',
    tech=4, level=4, cost=4,
    effects=[Effect(EK_health, ET_attack, amount=-16,
        on_hit=Effect(EK_sticky, ET_special, self=True,
            sticky=StickyInvoker('wounded', duration_turns(4))))
    ],
  ),
])
