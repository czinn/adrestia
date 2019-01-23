extends Node

onready var g = get_node('/root/global')

onready var back_button = $ui/back_button
onready var results_text = $ui/results_text
onready var game_history = $ui/game_history

var winner

func _ready():
	back_button.connect('pressed', self, 'on_back_button_pressed')
	var winners = g.backend.get_view().winners()
	var final_state = g.backend.get_state()
	game_history.view_player_id = g.backend.get_view().view_player_id
	game_history.history = final_state.history
	if winners.size() == 2:
		# Tie
		results_text.bbcode_text = '[b]You tied![/b]\nThis means you half-lost. Try harder next time.'
		winner = -1
	elif winners.has(0):
		# Won!
		results_text.bbcode_text = '[b]You won![/b]\nYour spells are very impressive. You must be very proud.'
		winner = 0
	else:
		# Loss
		results_text.bbcode_text = '[b]You lost.[/b]\nYou need more practice.'
		winner = 1

func on_back_button_pressed():
	g.scene_loader.goto_scene('title')
