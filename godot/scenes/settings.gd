extends Node

onready var g = get_node('/root/global')
onready var placeholder_button = $ui/placeholder_button
onready var credits_button = $ui/credits_button
onready var back_button = $ui/back_button

func _ready():
	credits_button.connect('pressed', g.scene_loader, 'goto_scene', ['credits'])
	back_button.connect('pressed', self, 'on_back_button_pressed')
	pass

func _notification(what):
	if what == MainLoop.NOTIFICATION_WM_QUIT_REQUEST:
		self.call_deferred('on_back_button_pressed')

func on_back_button_pressed():
	g.scene_loader.goto_scene('title', true)
