extends Control

onready var g = get_node('/root/global')

onready var texture = $vbox/texture
onready var label = $vbox/label
onready var animation_player = $animation_player

var spell = null setget set_spell

func _ready():
	pass

func _gui_event(event):
	if g.event_is_pressed(event) && spell != null:
		g.summon_spell_tooltip(self, spell)

func set_spell(spell_):
	spell = spell_
	redraw()

func flash():
	animation_player.play("flash")

func redraw():
	if g == null: return
	if spell == null: return

	texture.texture = g.get_spell_texture(spell.get_id())
	label.text = spell.get_name()
