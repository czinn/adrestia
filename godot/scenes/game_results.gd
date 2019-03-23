extends Node

onready var g = get_node('/root/global')

onready var back_button = $ui/back_button
onready var title_label = $ui/title_label
onready var detail_text = $ui/detail_text
onready var game_history = $ui/game_history

var winner

func _ready():
	back_button.connect('pressed', self, 'on_back_button_pressed')
	var final_state = g.backend.get_state()
	var winners = g.backend.get_state().winners()
	game_history.view_player_id = g.backend.get_view().view_player_id
	game_history.state = final_state

	if winners.size() == 2:
		# Tie
		g.sound.set_music('title')
		title_label.text = 'Draw.'
		detail_text.bbcode_text = "[center]It's a tie![/center]"
		winner = -1
	elif winners.size() == 0:
		g.sound.set_music('title')
		title_label.text = 'Victory!'
		detail_text.bbcode_text = '[center]By forfeit.[/center]'
		winner = game_history.view_player_id
	elif winners.has(game_history.view_player_id):
		# Won!
		g.sound.set_music('title')
		title_label.text = 'Victory!'
		detail_text.bbcode_text = '[center]Congratulations![/center]'
		winner = game_history.view_player_id
	else:
		# Loss
		g.sound.set_music('sad')
		title_label.text = 'Defeat.'
		detail_text.bbcode_text = '[center]Better luck next time.[/center]'
		winner = 1 - game_history.view_player_id

func _notification(what):
	if what == MainLoop.NOTIFICATION_WM_QUIT_REQUEST:
		self.call_deferred('on_back_button_pressed')

func on_back_button_pressed():
	g.sound.play_sound('button')
	g.scene_loader.goto_scene('title', true)
