extends Control

signal popup_closed(confirmed)

onready var g = get_node('/root/global')

onready var mouse_blocker = $mouse_blocker
onready var big_text_wnd = $nine_patch_rect
onready var big_text = $nine_patch_rect/rich_text_label
onready var yes_button = $nine_patch_rect/yes_button
onready var no_button = $nine_patch_rect/no_button

var text = "" setget set_text

func _ready():
	mouse_blocker.connect('gui_input', self, 'blocker_input')
	no_button.connect('pressed', self, 'no_button_pressed')
	yes_button.connect('pressed', self, 'yes_button_pressed')
	redraw()

func set_text(text_):
	text = text_
	redraw()

func no_button_pressed():
	get_parent().remove_child(self)
	emit_signal('popup_closed', false)

func yes_button_pressed():
	get_parent().remove_child(self)
	emit_signal('popup_closed', true)

func blocker_input(event):
	if g.event_is_pressed(event):
		no_button_pressed()

func redraw():
	if big_text == null: return
	big_text.bbcode_text = text
