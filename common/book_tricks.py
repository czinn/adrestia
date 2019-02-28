from rules_schema import *

book = Book('tricks', 'Book of Deception', [
  spell_tech(name='Learn Deception', text='Increase Deception by 1.'),

  Spell('1', 'Backfire',
      'Counter opponent\'s spell if they aren\'t learning a spell. Lose 3 health.',
      tech=1, level=1, cost=1,
      effects=[Effect(EK_health, ET_special, self=True, amount=-3)],
      counterspell=Selector(),
  ),

  Spell('2', 'Gom Jabbar',
    'Deal 3 damage. On hit: deal an additional 9 damage.',
    tech=2, level=2, cost=2,
    effects=[
      Effect(EK_health, ET_attack, amount=-3,
          on_hit=Effect(EK_health, ET_special, self=True, amount=-9))
    ],
  ),

  Spell('3', 'Embezzle',
    'If your opponent\'s spell is a shield, increase your mana regeneration by 2.',
    tech=3, level=3, cost=3,
    # This spell gets magically countered by the C++ code if the opponent's spell isn't a shield.
    effects=[effect_mana_regen(2)],
  ),

  Spell('4', 'Censure',
    'Your opponent takes 15 piercing damage if they learn a new spell next turn.',
    tech=4, level=4, cost=4,
    effects=[
    Effect(EK_sticky, ET_special, self=False,
        sticky=StickyInvoker('antitech_next', duration=duration_turns(1)))
    ],
  ),
])
