extends Control

onready var g = get_node('/root/global')

onready var texture = $texture
onready var duration_label = $duration_label
onready var animation_player = $animation_player

var sticky = null setget set_sticky

func _ready():
	pass

func _gui_event(event):
	if g.event_is_pressed(event) && sticky != null:
		g.summon_sticky_tooltip(self, sticky.sticky)

func set_sticky(sticky_):
	sticky = sticky_

func fadein():
	animation_player.play("fadein")
	redraw()

func appear():
	animation_player.play('appear')
	redraw()

func fadeout():
	animation_player.play("fadeout")
	redraw()

func redraw():
	if g == null: return
	if sticky == null: return

	texture.texture = g.get_sticky_texture(sticky.sticky.get_id())

	if sticky.amount != 0:
		duration_label.text = str(sticky.amount) + \
				('x' if sticky.sticky.get_stacks() else '')
	else:
		duration_label.text = ''
