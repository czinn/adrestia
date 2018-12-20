from rules_schema import *

book = Book('contrition', 'Book of Contrition', [
    spell_tech(name='Learn Contrition', text='Increase Contrition by 1.'),

    Spell('repent', 'Repent',
        "Gain 3 mana, lose 1 health.",
        tech=1, level=1, cost=1,
        effects=[
            Effect(EK_mana, ET_special, self=True, amount=3),
            Effect(EK_health, ET_special, self=True, amount=-1),
        ],
    ),
])
