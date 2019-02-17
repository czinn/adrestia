extends Node

onready var g = get_node('/root/global')

onready var back_button = $ui/back_button
onready var results_text = $ui/results_text
onready var game_history = $ui/game_history

var winner

func _ready():
	back_button.connect('pressed', self, 'on_back_button_pressed')
	var final_state = g.backend.get_state()
	var winners = g.backend.get_state().winners()
	game_history.view_player_id = g.backend.get_view().view_player_id
	game_history.history = final_state.history
	game_history.health_history = g.health_history
	print('Winners are:')
	print(winners)

	if winners.size() == 2:
		# Tie
		results_text.bbcode_text = '[b]You tied![/b]'
		winner = -1
	elif winners.size() == 0:
		results_text.bbcode_text = '[b]Someone forfeit![/b]'
		winner = -1
	elif winners.has(game_history.view_player_id):
		# Won!
		results_text.bbcode_text = '[b]You won![/b]'
		winner = game_history.view_player_id
	else:
		# Loss
		results_text.bbcode_text = '[b]You lost.[/b]'
		winner = 1 - game_history.view_player_id

func _notification(what):
	if what == MainLoop.NOTIFICATION_WM_QUIT_REQUEST:
		self.call_deferred('on_back_button_pressed')

func on_back_button_pressed():
	g.scene_loader.goto_scene('title', true)
