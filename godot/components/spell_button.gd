extends Control

signal pressed

onready var texture_button = $vbox/texture_button
onready var label = $vbox/label
onready var timer = $timer
var spell = null setget set_spell
var was_long_pressed = false
var down_pos = null

onready var g = get_node('/root/global')

func _ready():
	texture_button.connect('button_down', self, 'on_down')
	texture_button.connect('button_up', self, 'on_up')
	timer.connect('timeout', self, 'on_long_press')
	redraw()

func on_down():
	down_pos = get_viewport().get_mouse_position()
	timer.start()

func on_long_press():
	if get_viewport().get_mouse_position() != down_pos:
		return
	was_long_pressed = true
	timer.stop()
	var text = "[b]%s[/b]\n[i]Book[/i]: %d    [i]Level[/i]: %d    [i]Cost[/i]: %d\n%s" % [spell.get_name(), spell.get_tech(), spell.get_level(), spell.get_cost(), spell.get_text()]
	g.summon_tooltip(self, text)

func on_up():
	if not was_long_pressed:
		emit_signal('pressed')
		timer.stop()
	else:
		# Charles thinks auto-dismiss-on-release is nice based on testing on
		# desktop, but if mobile experience is different, remove this line.
		g.close_tooltip()
	was_long_pressed = false

func set_spell(spell_):
	spell = spell_
	redraw()

func redraw():
	if spell == null: return
	if label == null: return
	label.text = spell.get_name()
