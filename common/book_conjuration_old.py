from rules_schema import *

book = Book('conjuration_old', 'Old Book of Conjuration', [
	spell_tech(name='Learn Conjuration', text='Increase conjuration by 1.'),

	Spell('attack_1', 'Fiery Poke',
		'Deal 1 damage.',
		tech=0, level=1, cost=1,
		effects=[effect_attack(1)]
	),

	Spell('shield_1', 'Ripple Shield',
		'Absorbs 3 damage. Breaks at the end of the turn.',
		tech=0, level=1, cost=2,
		effects=[effect_shield(3)]
	),

	Spell('mana_1', 'Concentration',
		'Increase mana generation by 1.',
		tech=0, level=1, cost=2,
		effects=[effect_shield(3)]
	),

	Spell('poison_1', 'Poison',
		'Deal 1 damage per turn for the rest of the game.',
		tech=2, level=2, cost=5,
		effects=[Effect(EK_sticky, ET_poison, self=False,
			sticky=StickyInvoker('poison', duration_game))],
	),

	Spell('shield_2', 'Tower Shield',
		"Block 4 damage; lasts for 2 turns.",
		tech=2, level=3, cost=3,
		effects=[effect_shield(4, duration_turns(2))],
	),

	Spell('shield_3', 'Greater Shield',
		"Block 8 damage.",
		tech=3, level=3, cost=5,
		effects=[effect_shield(8)],
	),

	Spell('damage_2', 'Flame Spear',
		"Deal 5 damage.",
		tech=3, level=3, cost=3,
		effects=[effect_attack(5)],
	),

	Spell('chain_1', 'Gathering Storm',
		"Deal 3 damage, plus 1 for each time you previously cast this spell.",
		tech=3, level=4, cost=4,
		effects=[
			effect_attack(3),
			Effect(EK_sticky, ET_special, self=True, sticky=StickyInvoker('storm', duration_game, -1)),
		],
	),

	Spell('damage_3', 'Lightning Bolt',
		"Deal 7 damage.",
		tech=4, level=5, cost=4,
		effects=[effect_attack(7)],
	),

	Spell('endgame_1', 'Catalyst Cannon',
		"Deal 4 damage. Enemy takes 1 damage for each additional spell they cast this turn.",
		tech=5, level=7, cost=4,
		effects=[
			Effect(EK_health, ET_constant, amount=-4),
			Effect(EK_sticky, ET_special, sticky=StickyInvoker('spell_poison', duration_turns(1))),
		],
	),
])
