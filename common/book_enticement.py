from rules_schema import *

book = Book('enticement', 'Book of Enticement', [
    spell_tech(name='Learn Enticement', text='Increase Enticement by 1.'),

    Spell('mana_1', 'Siphon',
        'Drain 1 mana each turn for 2 turns.',
        tech=1, level=1, cost=2,
        effects=[
            Effect(EK_sticky, ET_special, sticky=StickyInvoker('suck_mana',
                duration_turns(2))),
        ],
    ),

		Spell('attack_1', 'Vampiric Blast',
				'Deal 8 damage. Gain 4 health.',
				tech=1, level=1, cost=4,
				effects=[
					effect_attack(8),
					Effect(EK_health, ET_special, self=True, amount=4),
				]
		),

    Spell('mana_2', 'Mana Flux',
        'Destroy 3 of your opponent\'s mana.',
        tech=2, level=2, cost=3,
        effects=[Effect(EK_mana, ET_special, self=False, amount=-3)],
    ),

		Spell('attack_2', 'Mana Blast',
				'Deal 12 damage. Your opponent gains 10 mana.',
				tech=2, level=2, cost=3,
				effects=[
					effect_attack(12),
					Effect(EK_mana, ET_special, self=False, amount=10),
				]
		),
])
