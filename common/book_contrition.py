from rules_schema import *

book = Book('contrition', 'Book of Contrition', [
    spell_tech(name='Learn Contrition', text='Increase Contrition by 1.'),

    Spell('mana', 'Repent',
        "Lose 3 health. Increase mana regeneration by 1.",
        tech=1, level=1, cost=1,
        effects=[
            Effect(EK_mana_regen, ET_special, self=True, amount=1),
            Effect(EK_health, ET_special, self=True, amount=-3),
        ],
    ),

    Spell('explosion', 'Explosion',
        "Deal 6 damage to both players.",
        tech=1, level=1, cost=2,
        effects=[
            Effect(EK_health, ET_attack, self=True, amount=-6),
						effect_attack(6),
        ],
    ),

    Spell('damage', 'Harm',
        "Deal 10 damage. Decrease your mana regeneration by 1.",
        tech=2, level=2, cost=3,
        effects=[
            Effect(EK_mana_regen, ET_special, self=True, amount=-1),
						effect_attack(10),
        ],
    ),

    Spell('shield', 'Blood Wall',
        "Block 11 damage this turn. Poison yourself, dealing 1 daamge per turn for the rest of the game.",
        tech=2, level=2, cost=3,
        effects=[
						Effect(EK_sticky, ET_poison, self=True,
							sticky=StickyInvoker('poison', duration_game)),
						effect_shield(11),
        ],
    ),
])
