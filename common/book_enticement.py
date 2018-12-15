from rules_schema import *

book = Book('enticement', 'Book of Enticement', [
    spell_tech(name='Learn Enticement', text='Increase Enticement by 1.'),

    Spell('suck_mana', 'Suck Mana',
        "Drain 1 mana each turn for 2 turns.",
        tech=1, level=1, cost=2,
        effects=[
            Effect(EK_sticky, ET_special, sticky=StickyInvoker('suck_mana',
                duration_turns(2))),
        ],
    ),
])
