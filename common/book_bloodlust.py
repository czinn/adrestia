from rules_schema import *

book = Book('bloodlust', 'Book of Bloodlust', [
	spell_tech(name='Learn Bloodlust', text='Increase Bloodlust by 1.'),

	Spell('attack_1', 'Razor Wind',
		'Deal 5 damage.',
		tech=1, level=1, cost=2,
		effects=[effect_attack(4)],
	),

	Spell('attack_2', 'Razor Storm',
		'Deal 13 damage.',
		tech=1, level=1, cost=4,
		effects=[effect_attack(4)],
	),

	Spell('boost', 'Fury',
		'Your attacks deal 3 extra damage this turn.',
		tech=2, level=2, cost=2,
		effects=[effect_shield(4)],
		counterspell=Selector(effect_type=['shield']),
	),

	Spell('special', 'Bloodlust',
		'All spells you cast deal +2 damage for the rest of the game.',
		tech=3, level=3, cost=5,
		effects=[
			Effect(EK_sticky, ET_special, self=True,
				sticky=StickyInvoker('bloodlust', duration_game, amount=2)),
		]
	),
])
