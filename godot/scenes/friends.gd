extends Node

onready var g = get_node('/root/global')

onready var avatar_profile_scene = preload('res://components/avatar_profile.tscn')

onready var friend_list = $ui/scroll_container/v_box_container
onready var back_button = $ui/back_button
onready var add_friend_button = $ui/add_friend_button
onready var offline_warning = $ui/offline_warning
onready var challenge_button = $ui/challenge_button

func _ready():
	back_button.connect('pressed', self, 'on_back_button_pressed')
	add_friend_button.connect('pressed', self, 'on_add_friend_button_pressed')
	g.clear_children(friend_list)
	g.network.register_handlers(self, 'on_connected', 'on_disconnected', 'on_out_of_date')
	offline_warning.visible = false

func _notification(what):
	if what == MainLoop.NOTIFICATION_WM_QUIT_REQUEST:
		self.call_deferred('on_back_button_pressed')

func on_add_friend_button_pressed():
	var friend_code = yield(g.summon_text_entry('Friend Code (FC):', ''), 'popup_closed')
	if friend_code:
		g.network.follow_user(friend_code, funcref(self, 'on_friend_added'))

func on_friend_added(response):
	g.network.get_friends(funcref(self, 'on_get_friends_list'))

func on_get_friends_list(response):
	print(response)
	for friend in response.friends:
		var profile = avatar_profile_scene.instance()
		friend_list.add_child(profile)
		profile.name_label.text = friend.user_name
		profile.fc_label.text = 'FC: ' + friend.friend_code
		profile.buttons.visible = true
		profile.challenge_button.connect('pressed', self, 'on_challenge_friend', [friend])
		print(friend.is_online)
		print(typeof(friend.is_online))
		if friend.is_online:
			profile.online_label.text = 'Online'
		else:
			profile.online_label.text = 'Last online ' + friend.last_login.split(' ')[0]

func on_challenge_friend(friend):
	print('Challenging friend %s' % [friend.friend_code])

func on_back_button_pressed():
	g.scene_loader.goto_scene('title', true)

func on_connected():
	g.network.get_friends(funcref(self, 'on_get_friends_list'))
	offline_warning.visible = false

func on_disconnected():
	g.clear_children(friend_list)
	offline_warning.visible = true
	offline_warning.text = "Offline"

func on_out_of_date():
	g.clear_children(friend_list)
	offline_warning.visible = true
	offline_warning.text = "Client out of date"
