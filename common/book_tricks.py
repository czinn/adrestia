from rules_schema import *

book = Book ('tricks', 'Book of Deception', [
  spell_tech(name='Learn Deception', text='Increase Deception by 1.'),

  Spell('shield', 'Flickering Shield',
      'Block 13 damage. Lasts one step.',
      tech=1, level=1, cost=1,
      effects=[effect_shield(13, duration=duration_steps(1))],
  ),

  Spell('antitech', 'Censure',
      'Your opponent loses 6 health if they learn a new spell next turn',
      tech=2, level=2, cost=2,
      effects=[
        Effect(EK_sticky, ET_special, self=False,
          sticky=StickyInvoker('antitech_next', duration=duration_turns(1)))
      ],
  ),

  Spell('cancel', 'Backfire',
      'Counter opponent\'s spell if it isn\'t a knowledge spell. Lose 2 health.',
      tech=3, level=3, cost=1,
      effects=[Effect(EK_health, ET_special, self=True, amount=-2)],
      counterspell=Selector(),
  ),

  Spell('fury', 'Fury',
      'Deal damage equal to the health you\'ve lost.',
      tech=4, level=4, cost=4,
      # Amount for this spell gets magically determined by the C++ code
      effects=[effect_attack(0)],
  ),
])
