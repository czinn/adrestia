extends Node

onready var g = get_node('/root/global')
onready var play_button = $ui/play_button

func _ready():
	get_tree().set_auto_accept_quit(true)
	get_tree().set_quit_on_go_back(true)
	play_button.connect('pressed', self, 'on_play_button_pressed')

func on_play_button_pressed():
	g.scene_loader.goto_scene('res://scenes/game.tscn')
