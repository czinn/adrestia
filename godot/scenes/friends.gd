extends Node

onready var g = get_node('/root/global')

onready var back_button = $ui/back_button

func _ready():
	back_button.connect('pressed', self, 'on_back_button_pressed')
	g.network.register_handlers(self, 'on_connected', 'on_disconnected', 'on_out_of_date')

func _notification(what):
	if what == MainLoop.NOTIFICATION_WM_QUIT_REQUEST:
		self.call_deferred('on_back_button_pressed')

func on_back_button_pressed():
	g.scene_loader.goto_scene('title', true)

func on_connected():
	pass

func on_disconnected():
	pass

func on_out_of_date():
	pass
