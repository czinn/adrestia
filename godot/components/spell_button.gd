extends Control

onready var texture_button = $vbox/texture_button
onready var label = $vbox/label
var spell = null setget set_spell

signal pressed

func _ready():
  texture_button.connect('pressed', self, 'on_pressed')
  redraw()

func on_pressed():
  emit_signal('pressed')

func set_spell(spell_):
  spell = spell_
  redraw()

func redraw():
  if spell == null: return
  if label == null: return
  label.text = spell.get_name()
