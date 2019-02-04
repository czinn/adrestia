from rules_schema import *

book = Book('contrition', 'Book of Contrition', [
	spell_tech(name='Learn Contrition', text='Increase Contrition by 1.'),

	Spell('mana', 'Repent',
		"Lose 5 health. Increase mana regeneration by 1.",
		tech=1, level=1, cost=0,
		effects=[
			Effect(EK_mana_regen, ET_special, self=True, amount=1),
			Effect(EK_health, ET_special, self=True, amount=-5),
		],
	),

	Spell('cancel', 'Shieldbreaker',
        "Counter opponent's spell if it is a shield.",
        tech=2, level=2, cost=1,
        effects=[],
        counterspell=Selector(effect_type=['shield']),
	),

	Spell('damage', 'Harm',
		"Deal 12 damage. Decrease your mana regeneration by 1.",
		tech=3, level=3, cost=3,
		effects=[
			Effect(EK_mana_regen, ET_special, self=True, amount=-1),
			effect_attack(12),
		],
	),

	Spell('explosion', 'Big Blast',
		"Deal 3 damage, plus 3 damage per turn for three turns.",
		tech=4, level=4, cost=3,
		effects=[
			effect_attack(3),
			Effect(EK_sticky, ET_attack,
				sticky=StickyInvoker('exploded', duration_turns(3))),
		],
	),
])
