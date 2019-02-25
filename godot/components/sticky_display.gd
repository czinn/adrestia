extends Control

onready var g = get_node('/root/global')

onready var texture = $texture
onready var glow_texture = $texture/glow_texture
onready var duration_label = $duration_label
onready var animation_player = $animation_player

var sticky = null
var sticky_amount = 0
var old_amount = null

func _ready():
	pass

func _gui_event(event):
	if g.event_is_pressed(event) && sticky != null:
		g.summon_sticky_tooltip(self, sticky)

func set_sticky_instance(sticky_instance):
	sticky = sticky_instance.sticky
	sticky_amount = sticky_instance.amount

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

	texture.texture = g.get_sticky_texture(sticky.get_id())
	glow_texture.texture = texture.texture
	
	if old_amount != null && old_amount != sticky_amount:
		g.summon_delta(duration_label, sticky_amount - old_amount, Color(0, 0, 0))
	old_amount = sticky_amount

	if sticky_amount != 0 && !sticky.get_stacks():
		duration_label.text = str(sticky_amount)
	else:
		duration_label.text = ''
