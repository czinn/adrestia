extends Control

signal popup_closed(text)

onready var g = get_node('/root/global')

onready var mouse_blocker = $mouse_blocker
onready var line_edit = $ui/line_edit
onready var label = $ui/label
onready var done_button = $ui/done_button

var text
var default_text

func _ready():
	mouse_blocker.connect('gui_input', self, 'blocker_input')
	done_button.connect('pressed', self, 'done_button_pressed')
	label.text = text if text else 'Text input'
	line_edit.text = default_text if default_text else ''

func done_button_pressed():
	get_parent().remove_child(self)
	emit_signal('popup_closed', line_edit.text)

func no_button_pressed():
	get_parent().remove_child(self)
	emit_signal('popup_closed', null)

func blocker_input(event):
	if g.event_is_pressed(event):
		no_button_pressed()
