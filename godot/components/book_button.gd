extends Control

onready var button = $vbox/button
onready var checkmark = $vbox/button/checkmark
onready var label = $vbox/label
var book = null setget set_book
onready var checked = false setget set_checked

func _ready():
  redraw()

func set_book(book_):
  book = book_
  redraw()

func set_checked(checked_):
  checked = checked_
  redraw()

func redraw():
  if book == null: return
  if label == null: return
  label.text = book.get_name()
  checkmark.visible = checked
