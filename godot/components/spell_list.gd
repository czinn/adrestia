extends Control

onready var g = get_node('/root/global')
onready var hbox = $scroll_container/hbox
var book = null setget set_book

onready var spell_button_scene = preload('res://components/spell_button.tscn')

func _ready():
  if book == null:
    book = g.rules.get_book('conjuration')
  g.clear_children(hbox)
  redraw()

func set_book(book_):
  print('book set')
  book = book_
  redraw()

func redraw():
  print('spell list redrawing')
  print(hbox)
  print(book)
  if hbox == null: return
  if book == null: return

  g.clear_children(hbox)
  for spell_id in book.get_spells():
    var spell = g.rules.get_spell(spell_id)
    var spell_button = spell_button_scene.instance()
    spell_button.spell = spell
    hbox.add_child(spell_button)
