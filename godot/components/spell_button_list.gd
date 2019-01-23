extends Control

signal pressed(index, spell)
signal long_pressed(index, spell)

onready var g = get_node('/root/global')

onready var hbox = $hbox
var spells = null setget set_spells
var enabled_filter = null setget set_enabled_filter
var unlocked_filter = null setget set_unlocked_filter
var display_filter = null setget set_display_filter
var show_stats = false setget set_show_stats
var immediately_show_tooltip = false setget set_immediately_show_tooltip

func _ready():
	if spells == null:
		spells = []
	g.clear_children(hbox)
	redraw()

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

func set_immediately_show_tooltip(show_):
	immediately_show_tooltip = show_
	redraw()

func flash_spell(index):
	hbox.get_children()[index].flash()

func spell_countered(index):
	hbox.get_children()[index].countered()

func redraw():
	if hbox == null: return
	if spells == null: return

	g.clear_children(hbox)

	var spell_buttons = g.make_spell_buttons(spells, show_stats, display_filter, enabled_filter, unlocked_filter)
	for i in range(len(spell_buttons)):
		var spell_button = spell_buttons[i]
		var spell = spell_button.spell
		spell_button.immediately_show_tooltip = immediately_show_tooltip
		spell_button.connect('pressed', self, 'on_pressed', [i, spell])
		hbox.add_child(spell_button)

func on_pressed(index, spell):
	emit_signal('pressed', index, spell)
	redraw()
