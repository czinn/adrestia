extends Control

signal pressed

onready var g = get_node('/root/global')

onready var texture_button = $ui/vbox/texture_button
onready var glow_texture = $ui/vbox/texture_button/glow_texture
onready var label = $ui/vbox/label
onready var timer = $timer
onready var cost = $ui/cost
onready var mp_icon = $ui/cost/mp_icon
onready var mp_label = $ui/cost/mp_label
onready var unlock = $ui/unlock
onready var padlock = $ui/unlock/padlock
onready var animation_player = $animation_player

var spell = null setget set_spell
var enabled = true setget set_enabled
var locked = false
var show_stats = false
var unlockable = false
var immediately_show_tooltip = false
var appear_anim = false

var was_long_pressed = false

func _ready():
	texture_button.connect('button_down', self, 'on_down')
	texture_button.connect('button_up', self, 'on_up')
	timer.connect('timeout', self, 'on_long_press')
	redraw()
	if appear_anim:
		animation_player.play('appear')

func on_down():
	was_long_pressed = false
	timer.start()

func on_long_press():
	was_long_pressed = true
	timer.stop()
	g.summon_spell_tooltip(self, spell)

func on_up():
	timer.stop()
	if was_long_pressed:
		# Auto dismiss on release
		g.close_tooltip()
	elif immediately_show_tooltip:
		on_long_press()
	else:
		emit_signal('pressed')
	was_long_pressed = false

func set_spell(spell_):
	var changed = (spell == null or spell.get_id() != spell_.get_id())
	spell = spell_
	redraw()
	if changed and animation_player and appear_anim:
		animation_player.play('appear')

func set_enabled(enabled_):
	enabled = enabled_
	redraw()

func flash():
	animation_player.play('glow')

func countered():
	animation_player.play('countered')

func redraw():
	if spell == null: return
	if label == null: return
	if mp_label == null: return

	texture_button.texture_normal = g.get_spell_texture(spell.get_id())
	glow_texture.texture = texture_button.texture_normal
	
	var material_ = null if enabled else load('res://shaders/greyscale.material')
	texture_button.material = material_
	mp_icon.material = material_

	label.set('custom_colors/font_color', null if enabled else Color(0.75, 0.75, 0.75))
	label.text = spell.get_name()
	cost.visible = show_stats
	mp_label.text = str(spell.get_cost())

	if locked:
		unlock.visible = true
		if unlockable:
			padlock.texture = load('res://art-built/spell-lock-unlockable.png')
		else:
			padlock.texture = load('res://art-built/spell-lock.png')
	else:
		unlock.visible = false
