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
  book = book_
  redraw()

func redraw():
  if hbox == null: return
  if book == null: return

  g.clear_children(hbox)
  for spell_id in book.get_spells():
    var spell = g.rules.get_spell(spell_id)
    var spell_button = spell_button_scene.instance()
    spell_button.spell = spell
    hbox.add_child(spell_button)
    if handle_spell_clicked_object:
      spell_button.connect('pressed', handle_spell_clicked_object, handle_spell_clicked_method, [spell])

var handle_spell_clicked_object = null
var handle_spell_clicked_method = null

func handle_spell_clicked(object, method):
  handle_spell_clicked_object = object
  handle_spell_clicked_method = method
  redraw()
