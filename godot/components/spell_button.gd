extends Control

signal pressed

onready var g = get_node('/root/global')

onready var texture_button = $vbox/texture_button
onready var label = $vbox/label
onready var timer = $timer
onready var cost = $cost
onready var mp_icon = $cost/mp_icon
onready var mp_label = $cost/mp_label
onready var unlock = $unlock
onready var tech_label = $unlock/tech_label
onready var level_label = $unlock/level_label

var spell = null setget set_spell
var enabled = true setget set_enabled
var mana_enabled = true setget set_mana_enabled
var show_stats = true setget set_show_stats

var was_long_pressed = false
var down_pos = null

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
	var text = "[b]%s[/b]\n[i]Tech[/i]: %d    [i]Level[/i]: %d    [i]Cost[/i]: %d\n%s" % [spell.get_name(), spell.get_tech(), spell.get_level(), spell.get_cost(), spell.get_text()]
	g.summon_tooltip(self, text)

func on_up():
	if not was_long_pressed:
		if get_viewport().get_mouse_position() != down_pos:
			return
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

func set_mana_enabled(mana_enabled_):
	mana_enabled = mana_enabled_
	redraw()

func set_show_stats(show_stats_):
	show_stats = show_stats_
	redraw()

func redraw():
	if spell == null: return
	if label == null: return
	if mp_label == null: return

	texture_button.texture_normal = g.load_or(
			'res://art-built/spells/%s.png' % spell.get_id(),
			'res://art-built/spells/placeholder.png')

	texture_button.material = null if enabled else load('res://shaders/greyscale.material')
	label.set('custom_colors/font_color', null if enabled else Color(0.75, 0.75, 0.75))
	label.text = spell.get_name()

	var show_cost = enabled or spell.is_tech_spell()

	cost.visible = show_stats and show_cost
	mp_label.text = str(spell.get_cost())
	mp_icon.material = null if mana_enabled else load('res://shaders/greyscale.material')

	unlock.visible = show_stats and not show_cost
	tech_label.text = str(spell.get_tech())
	level_label.text = str(spell.get_level())
