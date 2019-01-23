extends Control

signal pressed

onready var g = get_node('/root/global')

onready var texture_button = $vbox/texture_button
onready var glow_texture = $vbox/texture_button/glow_texture
onready var label = $vbox/label
onready var timer = $timer
onready var cost = $cost
onready var mp_icon = $cost/mp_icon
onready var mp_label = $cost/mp_label
onready var unlock = $unlock
onready var tech_icon = $unlock/tech_icon
onready var tech_label = $unlock/tech_label
onready var level_label = $unlock/level_label
onready var animation_player = $animation_player

var spell = null setget set_spell
var enabled = true setget set_enabled
var show_stats = true setget set_show_stats
var show_unlock = true setget set_show_unlock
var immediately_show_tooltip = false

var was_long_pressed = false

func _ready():
	texture_button.connect('button_down', self, 'on_down')
	texture_button.connect('button_up', self, 'on_up')
	timer.connect('timeout', self, 'on_long_press')
	redraw()

func on_down():
	was_long_pressed = false
	timer.start()

func on_long_press():
	was_long_pressed = true
	timer.stop()
	g.summon_spell_tooltip(self, spell)

func on_up():
	if not was_long_pressed:
		if immediately_show_tooltip:
			on_long_press()
		else:
			timer.stop()
			if enabled:
				emit_signal('pressed')
	else:
		# Charles thinks auto-dismiss-on-release is nice based on testing on
		# desktop, but if mobile experience is different, remove this line.
		g.close_tooltip()
	was_long_pressed = false

func set_spell(spell_):
	spell = spell_
	redraw()

func set_enabled(enabled_):
	enabled = enabled_
	redraw()

func set_show_stats(show_stats_):
	show_stats = show_stats_
	redraw()

func set_show_unlock(show_unlock_):
	show_unlock = show_unlock_
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
	#tech_icon.material = material_

	label.set('custom_colors/font_color', null if enabled else Color(0.75, 0.75, 0.75))
	label.text = spell.get_name()

	var show_cost = enabled or spell.is_tech_spell()

	cost.visible = show_stats
	#unlock.visible = show_stats and show_unlock

	mp_label.text = str(spell.get_cost())
	tech_label.text = str(spell.get_tech())
	level_label.text = str(spell.get_level())
