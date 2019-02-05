from rules_schema import *

book = Book('enticement', 'Book of Enticement', [
	spell_tech(name='Learn Enticement', text='Increase Enticement by 1.'),

	Spell('mana', 'Mana Droplets',
		'Block 3 damage this turn. Increase mana regeneration by 1.',
		tech=1, level=1, cost=3,
		effects=[effect_shield(3), effect_mana_regen(1)],
	),

	Spell('shield', 'Ripple Shield',
		'Block 5 damage this turn.',
		tech=2, level=2, cost=1,
		effects=[effect_shield(5)],
	),

	Spell('attack', 'Greater Fireblast',
		'Deal 13 damage.',
		tech=3, level=3, cost=4,
		effects=[effect_attack(13)],
	),

	Spell('drain', 'Mana Drain',
		'Your opponent loses 1 mana regeneration per turn for the rest of the game.',
		tech=4, level=4, cost=7,
		effects=[Effect(EK_sticky, ET_special, self=False, sticky=StickyInvoker('mana_drain', duration_game))],
	),
])
