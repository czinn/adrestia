extends Control

onready var animation_player = $animation_player
onready var label = $label

func _ready():
	label.visible = false

func play_text_and_color(text, color, fadeup=true):
	label.visible = true
	label.text = text
	label.add_color_override("font_color", color)
	animation_player.play("fadeup" if fadeup else "fadedown")