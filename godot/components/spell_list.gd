extends Control

signal pressed(index, spell)
signal long_pressed(index, spell)

onready var spell_button_scene = preload('res://components/spell_button.tscn')

onready var g = get_node('/root/global')

onready var scroll_container = $scroll_container
onready var hbox = $scroll_container/hbox
var spells = null setget set_spells
var enabled_filter = null setget set_enabled_filter
var unlocked_filter = null setget set_unlocked_filter
var display_filter = null setget set_display_filter
var show_stats = null setget set_show_stats

func _ready():
	scroll_container.connect('scroll_started', self, 'on_scroll_started')
	if spells == null:
		spells = []
	g.clear_children(hbox)
	redraw()

func on_scroll_started():
	g.close_tooltip()

func set_spells(spells_):
	spells = spells_
	redraw()

func set_enabled_filter(filter_):
	enabled_filter = filter_
	redraw()

func set_unlocked_filter(filter_):
	unlocked_filter = filter_
	redraw()

func set_display_filter(filter_):
	display_filter = filter_
	redraw()

func set_show_stats(show_stats_):
	show_stats = show_stats_
	redraw()

func redraw():
	if hbox == null: return
	if spells == null: return

	g.clear_children(hbox)
	var index = 0
	for spell_id in spells:
		var spell = g.rules.get_spell(spell_id)
		if display_filter != null and not display_filter.call_func(spell):
			continue
		var spell_button = spell_button_scene.instance()
		spell_button.show_stats = show_stats
		spell_button.enabled = enabled_filter == null or enabled_filter.call_func(spell)
		spell_button.show_unlock = \
				unlocked_filter == null or not unlocked_filter.call_func(spell)
		# Set the spell last so that we don't redraw so many times
		spell_button.spell = spell
		spell_button.connect('pressed', self, 'on_pressed', [index, spell])
		hbox.add_child(spell_button)
		index += 1

func on_pressed(index, spell):
	emit_signal('pressed', index, spell)
	redraw()
