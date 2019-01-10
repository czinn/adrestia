from rules_schema import *

book = Book ('tricks', 'Book of Deception', [
	spell_tech(name='Learn Deception', text='Increase Deception by 1.'),

	Spell('cancel', 'Backfire',
			'Counter opponent\'s spell. Deal 4 damage.',
			tech=1, level=1, cost=4,
			effects=[effect_attack(4)],
			counterspell=Selector(),
	),

	Spell('shield', 'Flickering Shield',
			'Block 13 damage. Lasts one step.',
			tech=2, level=2, cost=3,
			effects=[effect_shield(13, duration=duration_steps(1))],
	),

	Spell('antispell', 'Interdict',
			'Both players lose 6 health for each additional spell they cast this turn.',
			tech=3, level=3, cost=4,
			effects=[
				Effect(EK_sticky, ET_special, self=True,
					sticky=StickyInvoker('antispell', duration=duration_turns(1))),
				Effect(EK_sticky, ET_special, self=False,
					sticky=StickyInvoker('antispell', duration=duration_turns(1)))
			],
	),

	Spell('antitech', 'Censure',
			'Your opponent loses 5 health if they gain knowledge of a book next turn.',
			tech=4, level=4, cost=2,
			effects=[
				Effect(EK_sticky, ET_special, self=False,
					sticky=StickyInvoker('antitech_next', duration=duration_turns(1)))
			],
	),
])
