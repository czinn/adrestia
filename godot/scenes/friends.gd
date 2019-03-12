extends Node

onready var g = get_node('/root/global')

onready var back_button = $ui/back_button
onready var add_friend_button = $ui/add_friend_button

func _ready():
	back_button.connect('pressed', self, 'on_back_button_pressed')
	add_friend_button.connect('pressed', self, 'on_add_friend_button_pressed')
	g.network.register_handlers(self, 'on_connected', 'on_disconnected', 'on_out_of_date')

func _notification(what):
	if what == MainLoop.NOTIFICATION_WM_QUIT_REQUEST:
		self.call_deferred('on_back_button_pressed')

func on_add_friend_button_pressed():
	g.network.get_user_profile('8d07d9da8f42348cf46443984c862c49', funcref(self, 'print_response'))

func print_response(response):
	print(response)

func on_back_button_pressed():
	g.scene_loader.goto_scene('title', true)

func on_connected():
	pass

func on_disconnected():
	pass

func on_out_of_date():
	pass
