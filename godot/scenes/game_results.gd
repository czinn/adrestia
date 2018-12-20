extends Node

onready var g = get_node('/root/global')

onready var back_button = $ui/back_button
onready var results_text = $ui/results_text

func _ready():
	back_button.connect('pressed', self, 'on_back_button_pressed')
	var winners = g.backend.get_view().winners()
	if winners.size() == 2:
		# Tie
		results_text.bbcode_text = '[b]You tied![/b]\nThis means you half-lost. Try harder next time.'
	elif winners.has(0):
		# Won!
		results_text.bbcode_text = '[b]You won![/b]\nYour spells are very impressive. You must be very proud.'
	else:
		# Loss
		results_text.bbcode_text = '[b]You lost.[/b]\nYou need more practice.'

func on_back_button_pressed():
	g.scene_loader.goto_scene('title')
