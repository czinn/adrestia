from typing import List, Dict, Optional
import json
import sys

from rules_schema import *
from stickies import stickies_list

# we could DRY this out with __import__, but then mypy doesn't go into imports
from book_bloodlust import book as book_bloodlust
from book_conjuration import book as book_conjuration
from book_conjuration_old import book as book_conjuration_old
from book_contrition import book as book_contrition
from book_enticement import book as book_enticement
from book_refinement import book as book_refinement
from book_regulation import book as book_regulation
from book_regulation import old_book as book_regulation_old
from book_tricks import book as book_tricks

version = [2, 0, 0]
library: List[Book] = [
  book_conjuration, # Wild Wild West
  book_regulation, # The Turtle
  book_contrition, # The Emo
  book_enticement, # The Rogue
  book_refinement, # The BIG One
  book_bloodlust, # The Berserker
  book_tricks, # The Tricky One
]

for book in library:
  for spell in book.spells:
    spell.id_ = f'{book.id_}_{spell.sub_id}'
    spell.book = book.id_

tech_placeholder = spell_tech(name='Learn Spell', text='Learn some spell from some book.')
tech_placeholder.id_ = 'tech'
tech_placeholder.book = 'none' # should probably not cause problems

stickies_built = [deconstruct(sticky) for sticky in stickies_list]
spells = deconstruct(
  sum((book.spells for book in library), [tech_placeholder]),
  { Sticky: lambda sticky: sticky.id_ }
)
books = deconstruct(library, {Spell: lambda spell: spell.id_})

print(json.dumps({
  'version': version,
  'mana_cap': 10,
  'initial_mana_regen': 3,
  'initial_health': 40,
  'spell_cap': 3,
  'stickies': stickies_built,
  'spells': spells,
  'books': books,
}))

print(f'We have {len(stickies_built)} stickies, {len(spells)} spells, and {len(books)} books.', file=sys.stderr)
