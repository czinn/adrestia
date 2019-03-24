extends Node

onready var g = get_node('/root/global')

onready var back_button = $ui/back_button

func _ready():
	g.backend.register_started_callback(funcref(self, 'on_game_started'))
	back_button.connect('pressed', self, 'on_back_button_pressed')

func on_game_started():
	g.backend.register_started_callback(null)
	yield(get_tree(), 'idle_frame')
	g.scene_loader.goto_scene('game')

func on_back_button_pressed():
	g.sound.play_sound('button')
	var confirmed = yield(g.summon_confirm('[center]Are you sure you want to cancel the game?[/center]'), 'popup_closed')
	if confirmed:
		g.backend.leave_game()
		g.backend = null
		g.scene_loader.goto_scene('title', true)
