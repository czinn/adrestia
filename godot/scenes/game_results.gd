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
	game_history.health_history = g.health_history
	if winners.size() == 2:
		# Tie
		results_text.bbcode_text = '[b]You tied![/b]'
		winner = -1
	elif winners.has(0):
		# Won!
		results_text.bbcode_text = '[b]You won![/b]'
		winner = 0
	else:
		# Loss
		results_text.bbcode_text = '[b]You lost.[/b]'
		winner = 1

func _notification(what):
	print(what)
	if what == MainLoop.NOTIFICATION_WM_QUIT_REQUEST:
		self.call_deferred('on_back_button_pressed')

func on_back_button_pressed():
	g.scene_loader.goto_scene('title', true)
