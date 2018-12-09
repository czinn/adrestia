extends Control

onready var g = get_node('/root/global')

onready var spell_display_scene = preload('res://components/spell_display.tscn')

onready var scroll_container = $scroll_container
onready var hbox = $scroll_container/margin_container/hbox
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

func flash_spell(index):
	hbox.get_children()[index].flash()

func redraw():
	if hbox == null: return
	if spells == null: return

	g.clear_children(hbox)

	for i in range(len(spells)):
		var spell = g.rules.get_spell(spells[i])
		var spell_display = spell_display_scene.instance()
		hbox.add_child(spell_display)
		spell_display.spell = spell
