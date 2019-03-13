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
	var friend_code = yield(g.summon_text_entry('Friend Code (FC):', ''), 'popup_closed')
	if friend_code:
		g.network.follow_user(friend_code, funcref(self, 'on_friend_added'))

func on_friend_added(response):
	pass

func on_get_friends_list(response):
	print(response)

func on_back_button_pressed():
	g.scene_loader.goto_scene('title', true)

func on_connected():
	g.network.get_friends(funcref(self, 'on_get_friends_list'))

func on_disconnected():
	pass

func on_out_of_date():
	pass
