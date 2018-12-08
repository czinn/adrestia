extends Control

signal pressed(index, spell)
signal long_pressed(index, spell)

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

	var spell_buttons = g.make_spell_buttons(spells, show_stats, display_filter, enabled_filter, unlocked_filter)
	for i in range(len(spell_buttons)):
		var spell_button = spell_buttons[i]
		var spell = spell_button.spell
		spell_button.connect('pressed', self, 'on_pressed', [i, spell])
		hbox.add_child(spell_button)

func on_pressed(index, spell):
	emit_signal('pressed', index, spell)
	redraw()
