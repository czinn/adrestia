extends Control

signal pressed(index, spell)
signal long_pressed(index, spell)

onready var g = get_node('/root/global')

onready var slot_image_hbox = $background
onready var slot_image_template = $background/template

onready var hbox = $hbox
export var slots_to_show = 0
var spells = null setget set_spells
var enabled_filter = null setget set_enabled_filter
var unlocked_filter = null setget set_unlocked_filter
var display_filter = null setget set_display_filter
var show_stats = false setget set_show_stats
var immediately_show_tooltip = false setget set_immediately_show_tooltip

func _ready():
	if spells == null:
		spells = []
	slot_image_template.get_parent().remove_child(slot_image_template)
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
	g.clear_children(slot_image_hbox)
	for i in range(slots_to_show):
		var slot_image = slot_image_template.duplicate()
		slot_image.visible = true
		slot_image_hbox.add_child(slot_image)
		if i < len(spells):
			# Hide it
			slot_image.modulate = Color(1.0, 1.0, 1.0, 0.0)

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
