extends Control

signal pressed(index, spell)
signal long_pressed(index, spell)

onready var spell_button_scene = preload('res://components/spell_button.tscn')

onready var g = get_node('/root/global')

onready var scroll_container = $scroll_container
onready var hbox = $scroll_container/hbox
var spells = null setget set_spells

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

func redraw():
	if hbox == null: return
	if spells == null: return

	g.clear_children(hbox)
	var index = 0
	for spell_id in spells:
		var spell = g.rules.get_spell(spell_id)
		var spell_button = spell_button_scene.instance()
		spell_button.spell = spell
		spell_button.connect('pressed', self, 'on_pressed', [index, spell])
		hbox.add_child(spell_button)
		index += 1

func on_pressed(index, spell):
	emit_signal('pressed', index, spell)
	redraw()
