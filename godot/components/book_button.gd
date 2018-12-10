extends Control

signal pressed

onready var g = get_node('/root/global')

onready var button = $vbox/button
onready var checkmark = $vbox/button/checkmark
onready var label = $vbox/label
var book = null setget set_book
var checked = false setget set_checked

func _ready():
	button.connect('pressed', self, 'on_pressed')
	redraw()

func on_pressed():
	emit_signal('pressed')

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
	button.texture_normal = g.get_book_texture(book.get_id())
