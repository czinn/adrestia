from rules_schema import *

book = Book('conjuration', 'Book of Conjuration', [
    spell_tech(name='Learn Conjuration', text='Increase Conjuration by 1.'),

    Spell('attack_1', 'Flame Strike',
        'Deal 3 damage.',
        tech=1, level=1, cost=1,
        effects=[effect_attack(3)],
    ),

    Spell('shield_1', 'Ripple Shield',
        'Absorb 8 damage. Breaks at the end of the turn.',
        tech=1, level=1, cost=3,
        effects=[effect_shield(8)],
    ),

    Spell('mana_1', 'Fiery Spirit',
        'Increase mana regeneration by 1.',
        tech=2, level=2, cost=2,
        effects=[effect_mana_regen(1)],
    ),

    Spell('attack_2', 'Erupt',
        'Deal 7 damage.',
        tech=2, level=2, cost=3,
        effects=[effect_attack(7)],
    ),
])
