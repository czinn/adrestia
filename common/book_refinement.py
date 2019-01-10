from rules_schema import *

book = Book('refinement', 'Book of Refinement', [
	spell_tech(name='Learn Refinement', text='Increase Refinement by 1.'),

	Spell('mana', 'Greater Forge',
		'Increase mana regeneration by 2.',
		tech=1, level=1, cost=4,
		effects=[effect_mana_regen(2)],
	),

	Spell('long_shield', 'Ablative Matrix',
		'Block 6 damage. Lasts until destroyed.',
		tech=2, level=2, cost=3,
		effects=[effect_shield(6, duration_game)],
	),
		
	Spell('boost', 'Dragonskin',
		'Reduce incoming attacks by 1 damage for the rest of the game.',
		tech=3, level=3, cost=6,
		effects=[Effect(EK_sticky, ET_shield, self=True, sticky=StickyInvoker('damper', duration_game, 1))],
	),

	Spell('attack', 'Hellhammer',
		'Deal 9 damage.',
		tech=4, level=4, cost=4,
		effects=[effect_attack(9)],
	),
])

old_book = Book('refinement_old', 'Old Book of Refinement', [
	spell_tech(name='Learn Refinement', text='Increase Refinement by 1.'),

	Spell('mana_1', 'Forge',
		"Increase mana regeneration by 1.",
		tech=0, level=0, cost=3,
		effects=[effect_mana_regen(1)],
	),

	Spell('shield_1', 'Iron Flare',
		"Block 1 damage.",
		tech=0, level=0, cost=1,
		effects=[effect_shield(1)],
	),

	Spell('attack_1', 'Hammer',
		"Deal 2 damage.",
		tech=1, level=1, cost=2,
		effects=[effect_attack(2)],
	),

	Spell('heal_1', 'Biosynthesis',
		"Heal 1 damage.",
		tech=1, level=1, cost=2,
		effects=[Effect(EK_health, ET_attack, self=True, amount=1)],
	),

	Spell('shield_2', 'Ripple Shield',
		"Block 3 damage.",
		tech=1, level=1, cost=2,
		effects=[effect_shield(3)],
	),

	Spell('mana_2', 'Greater Forge',
		"Increase mana generation by 2.",
		tech=2, level=2, cost=3,
		effects=[effect_mana_regen(2)],
	),

	Spell('long_shield', 'Ablative Matrix',
		"Block 2 damage. Lasts until destroyed.",
		tech=2, level=3, cost=2,
		effects=[effect_shield(2, duration_game)],
	),

	Spell('attack_2', 'Hellhammer',
		"Deal 3 damage.",
		tech=3, level=3, cost=2,
		effects=[effect_attack(3)],
	),

	Spell('boost_1', 'Dragonskin',
		"Reduce incoming attacks by 1 damage for the rest of the game.",
		tech=4, level=4, cost=10,
		effects=[Effect(EK_sticky, ET_shield, self=True, sticky=StickyInvoker('damper', duration_game, 1))],
	),

	# TODO: implement this
	Spell('shield_3', 'Combat Shield TODO',
		"Block 2 damage. If destroyed by attacks, deals 2 damage to opponent.",
		tech=4, level=5, cost=3,
		effects=[effect_shield(2, duration_game)],
	),
])
