extends Control

onready var g = get_node('/root/global')

onready var texture = $texture
onready var duration_label = $duration_label

var sticky = null setget set_sticky

func _ready():
	pass

func _gui_event(event):
	if g.event_is_pressed(event) && sticky != null:
		var t = "[b]" + sticky.sticky.get_name() + "[/b]\n" + sticky.sticky.get_text()
		g.summon_tooltip(self, t)

func set_sticky(sticky_):
	sticky = sticky_
	redraw()

func redraw():
	if g == null: return
	if sticky == null: return

	texture.texture = g.load_or(
			'res://art-built/stickies/%s.png' % sticky.sticky.get_id(),
			'res://art-built/stickies/placeholder.png')

	if sticky.amount != 0:
		duration_label.text = str(sticky.amount) + \
				('x' if sticky.sticky.get_stacks() else '')
	else:
		duration_label.text = ''
