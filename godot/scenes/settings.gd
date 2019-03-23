extends Node

onready var g = get_node('/root/global')
onready var online_status = $ui/online_status
onready var reset_account_button = $ui/reset_account_button
onready var change_name_button = $ui/change_name_button
onready var credits_button = $ui/credits_button
onready var back_button = $ui/back_button

func _ready():
	credits_button.connect('pressed', self, 'on_credits_button_pressed')
	back_button.connect('pressed', self, 'on_back_button_pressed')
	reset_account_button.connect('pressed', self, 'on_reset_account_button_pressed')
	change_name_button.connect('pressed', self, 'on_change_name_button_pressed')
	g.network.register_handlers(self, 'on_connected', 'on_disconnected', 'on_out_of_date')

func _notification(what):
	if what == MainLoop.NOTIFICATION_WM_QUIT_REQUEST:
		self.call_deferred('on_back_button_pressed')

func on_credits_button_pressed():
	g.sound.play_sound('button')
	g.scene_loader.goto_scene('credits')

func on_back_button_pressed():
	g.sound.play_sound('button')
	g.scene_loader.goto_scene('title', true)

func on_connected():
	online_status.text = 'Online as %s' % [g.user_name]
	pass

func on_disconnected():
	if g.user_name != null:
		online_status.text = 'Offline as %s' % [g.user_name]
		pass
	else:
		online_status.text = 'Offline'
		pass

func on_out_of_date():
	online_status.text = 'Out of date client. Update the app to play online!'
	pass

func on_reset_account_button_pressed():
	g.sound.play_sound('button')
	if not g.network.is_online():
		g.summon_notification('Not online.')
		return
	if yield(g.summon_confirm('This will clear all of your in-game progress. Are you sure?'), 'popup_closed') == true:
		g.network.deactivate_account(funcref(self, 'on_account_deactivated'))

func on_account_deactivated(response):
	g.auth_uuid = null
	g.auth_pwd = null
	g.multiplayer_wins = null
	g.network.peer.disconnect_from_host()
	g.summon_notification('All right, enjoy your new account.')
	return true

func on_change_name_button_pressed():
	g.sound.play_sound('button')
	if not g.network.is_online():
		g.summon_notification('Not online.')
		return
	var new_name = yield(g.summon_text_entry('New name:', g.user_name), 'popup_closed')
	if new_name and new_name != g.user_name:
		g.network.change_user_name(new_name, funcref(self, 'on_username_changed'))
	else:
		g.summon_notification('Name unchanged.')

func on_username_changed(response):
	g.sound.play_sound('button')
	g.user_name = response.user_name
	g.friend_code = response.friend_code
	g.summon_notification('Name changed to %s.' % [g.user_name])
	on_connected()
	return true
