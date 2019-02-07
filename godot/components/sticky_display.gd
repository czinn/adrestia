extends Control

onready var g = get_node('/root/global')

onready var texture = $texture
onready var glow_texture = $texture/glow_texture
onready var duration_label = $duration_label
onready var animation_player = $animation_player

var sticky = null setget set_sticky
var old_amount = null

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

func flash():
	animation_player.play('glow')

func redraw():
	if g == null: return
	if sticky == null: return

	texture.texture = g.get_sticky_texture(sticky.sticky.get_id())
	glow_texture.texture = texture.texture
	
	if old_amount != null && old_amount != sticky.amount:
		g.summon_delta(duration_label, sticky.amount - old_amount, Color(0, 0, 0))
	old_amount = sticky.amount

	if sticky.amount != 0 && !sticky.sticky.get_stacks():
		duration_label.text = str(sticky.amount)
	else:
		duration_label.text = ''
