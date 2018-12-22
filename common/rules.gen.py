from typing import List, Dict, Optional
import json
import sys

from rules_schema import *
from stickies import stickies_list

# we could DRY this out with __import__, but then mypy doesn't go into imports
from book_conjuration import book as book_conjuration
from book_conjuration_old import book as book_conjuration_old
from book_regulation import book as book_regulation
from book_regulation import old_book as book_regulation_old
from book_contrition import book as book_contrition
from book_enticement import book as book_enticement
from book_refinement import book as book_refinement
from book_tricks import book as book_tricks

library: List[Book] = [
    book_conjuration,
    #book_conjuration_old,
    book_regulation,
    #book_regulation_old,
    #book_contrition,
    #book_enticement,
    #book_refinement,
    book_tricks,
]

for book in library:
    for spell in book.spells:
        spell.id_ = f'{book.id_}_{spell.sub_id}'
        spell.book = book.id_

stickies_built = [deconstruct(sticky) for sticky in stickies_list]
spells = deconstruct(
    sum((book.spells for book in library), []),
    { Sticky: lambda sticky: sticky.id_ }
)
books = deconstruct(library, {Spell: lambda spell: spell.id_})

print(json.dumps({
    'mana_cap': 10,
    'initial_mana_regen': 5,
    'initial_health': 25,
    'stickies': stickies_built,
    'spells': spells,
    'books': books,
}))

print(f'We have {len(stickies_built)} stickies, {len(spells)} spells, and {len(books)} books.', file=sys.stderr)
