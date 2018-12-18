from rules_schema import *

book = Book('regulation', 'Book of Regulation', [
    spell_tech(name='Learn Regulation', text='Increase Regulation by 1.'),
    Spell('mana_1', 'Focus',
        "Deal 2 damage. Increase mana regeneration by 1.",
        tech=1, level=1, cost=2,
        effects=[effect_attack(2), effect_mana_regen(1)],
    ),

    Spell('mana_2', 'Mana Crystal',
        "Gain 4 mana next turn.",
        tech=2, level=2, cost=2,
        effects=[Effect(EK_mana, ET_special, self=True, amount=4)],
    ),

    Spell('cancel_1', 'Shieldbreaker',
        "Counter opponent's spell if it is a shield. Block 4 damage this turn.",
        tech=3, level=3, cost=2,
        effects=[effect_shield(4)],
        counterspell=Selector(effect_type=['shield']),
    ),

    Spell('attack_1', 'Piercing Shot',
        "Deal 5 damage. Cannot be blocked by shields.",
        tech=4, level=4, cost=3,
        effects=[Effect(EK_health, ET_constant, amount=-5)],
    ),
])

old_book = Book('regulation_old', 'Old Book of Regulation', [
    spell_tech(name='Learn Regulation', text='Increase Regulation by 1.'),
    Spell('mana_1', 'Focus',
        "Increase mana regeneration by 1.",
        tech=0, level=1, cost=3,
        effects=[effect_mana_regen(1)],
    ),

    Spell('shield_1', 'Hex Shield',
        "Block 6 damage this turn.",
        tech=1, level=2, cost=4,
        effects=[effect_shield(6)],
    ),

    Spell('cancel_1', 'Shieldbreaker',
        "Counter a shield spell.",
        tech=2, level=3, cost=2,
        effects=[],
        counterspell=Selector(effect_type=['shield']),
    ),

    Spell('mana_2', 'Mana Crystal',
        "Gain 4 mana next turn.",
        tech=3, level=3, cost=3,
        effects=[Effect(EK_mana, ET_special, self=True, amount=4)],
    ),

    Spell('cancel_2', 'Negate',
        "Cancel an attack spell.",
        tech=2, level=4, cost=3,
        counterspell=Selector(effect_type=['attack']),
    ),

    Spell('damper', 'Damper',
        "Reduce incoming attacks this turn by 2 damage.",
        tech=3, level=4, cost=3,
        effects=[
            Effect(EK_sticky, ET_special, self=True,
                sticky=StickyInvoker('damper', duration_turns(1),
                    amount=2)),
        ],
    ),

    Spell('shield_2', 'Antidote',
        "Block 3 poison damage. Lasts 2 turns.",
        tech=4, level=4, cost=2,
        effects=[
            Effect(EK_sticky, ET_shield, self=True,
                sticky=StickyInvoker('poison_shield', duration_turns(2),
                    amount=3)),
        ],
    ),

    Spell('shield_3', 'Fortress',
        "Spells you cast this turn give you 2 shield each. Shields last until end of next turn.",
        tech=1, level=5, cost=3,
        effects=[
            Effect(EK_sticky, ET_shield, self=True,
                sticky=StickyInvoker('shields_from_spells',
                    duration_turns(1))),
        ],
    ),

    Spell('shield_4', 'Shield of Alaros',
        "Block next attack.",
        tech=4, level=5, cost=4,
        effects=[
            Effect(EK_sticky, ET_attack, self=True,
                sticky=StickyInvoker('super_shield', duration_game)),
        ],
    ),
])
