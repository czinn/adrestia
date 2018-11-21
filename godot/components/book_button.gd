extends Control

onready var texture_button = $vbox/texture_button
onready var label = $vbox/label
var book = null setget set_book

func _ready():
  redraw()

func set_book(book_):
  book = book_
  redraw()

func redraw():
  if book == null: return
  if label == null: return
  label.text = book.get_name()
