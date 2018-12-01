extends Node

onready var g = get_node('/root/global')
onready var back_button = $ui/back_button

func _ready():
	back_button.connect('pressed', self, 'on_back_button_pressed')

func on_back_button_pressed():
	g.scene_loader.goto_scene('title')
