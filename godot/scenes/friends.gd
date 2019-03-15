extends Node

const OnlineBackend = preload('res://backends/online.gd')

onready var g = get_node('/root/global')

onready var avatar_profile_scene = preload('res://components/avatar_profile.tscn')

onready var friend_list = $ui/scroll_container/v_box_container
onready var fc_label = $ui/fc_label
onready var back_button = $ui/back_button
onready var add_friend_button = $ui/add_friend_button
onready var offline_warning = $ui/offline_warning

func _ready():
	back_button.connect('pressed', self, 'on_back_button_pressed')
	add_friend_button.connect('pressed', self, 'on_add_friend_button_pressed')
	g.clear_children(friend_list)
	g.network.register_handlers(self, 'on_connected', 'on_disconnected', 'on_out_of_date')
	if g.friend_code:
		fc_label.text = 'Your Friend Code: %s' % [g.friend_code]
	else:
		fc_label.text = ''
	offline_warning.visible = false

func _notification(what):
	if what == MainLoop.NOTIFICATION_WM_QUIT_REQUEST:
		self.call_deferred('on_back_button_pressed')

func on_add_friend_button_pressed():
	var friend_code = yield(g.summon_text_entry('Friend Code (FC):', ''), 'popup_closed')
	if friend_code:
		g.network.follow_user(friend_code, funcref(self, 'on_friend_added'))

func on_friend_added(response):
	if response.api_code == 200:
		g.summon_notification("You are now following %s. Make sure they add you too." % [response.user_name])
		g.network.get_friends(funcref(self, 'on_get_friends_list'))
	else:
		g.summon_notification("Can't find that friend code.")

func on_get_friends_list(response):
	if len(response.friends) == 0:
		offline_warning.text = "No friends! Add one?"
		offline_warning.visible = true
	else:
		offline_warning.visible = false

	for friend in response.friends:
		var profile = avatar_profile_scene.instance()
		friend_list.add_child(profile)
		profile.name_label.text = friend.user_name
		profile.fc_label.text = 'FC: ' + friend.friend_code
		profile.buttons.visible = true
		profile.challenge_button.connect('pressed', self, 'on_challenge_friend', [friend])
		if friend.is_online:
			profile.online_label.text = 'Online'
		else:
			profile.online_label.text = 'Last online ' + friend.last_login.split(' ')[0]

func on_challenge_friend(friend):
	var confirmed = yield(g.summon_confirm('Challenge %s to a duel?' % [friend.user_name]), 'popup_closed')
	if confirmed:
		g.network.send_challenge(friend.friend_code, funcref(g.network, 'discard'))
		g.backend = OnlineBackend.new(g, friend.friend_code)
		g.scene_loader.goto_scene('game_book_select')

func on_back_button_pressed():
	g.scene_loader.goto_scene('title', true)

func on_connected():
	fc_label.text = 'Your Friend Code: %s' % [g.friend_code]
	g.network.get_friends(funcref(self, 'on_get_friends_list'))
	offline_warning.visible = false

func on_disconnected():
	g.clear_children(friend_list)
	offline_warning.text = "Offline"
	offline_warning.visible = true

func on_out_of_date():
	g.clear_children(friend_list)
	offline_warning.text = "Client out of date"
	offline_warning.visible = true
