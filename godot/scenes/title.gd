extends Node

onready var g = get_node('/root/global')
onready var online_status = $ui/online_status
onready var play_button = $ui/play_button
onready var placeholder_button = $ui/placeholder_button
onready var tutorial_button = $ui/tutorial_button
onready var animation_player = $animation_player

const TutorialOverlay = preload('res://components/tutorial_overlay.tscn')
const RandomAiBackend = preload('res://backends/random_ai.gd')
const TutorialBackend = preload('res://backends/tutorial.gd')

func _ready():
	var unique_id = OS.get_unique_id()
	if unique_id:
		print('Unique ID is: %s' % [unique_id])
	else:
		print('Failed to get unique id')
	get_tree().set_auto_accept_quit(true)
	get_tree().set_quit_on_go_back(true)
	play_button.connect('pressed', self, 'on_play_button_pressed')
	tutorial_button.connect('pressed', self, 'on_tutorial_button_pressed')
	if not g.loaded:
		g.loaded = true
		initialize()
		animation_player.play('fade_in')
		yield(animation_player, 'animation_finished')
	get_tree().set_auto_accept_quit(true)

func initialize():
	g.load()
	print(OS.get_user_data_dir())
	g.network.establish_connection(funcref(self, 'network_ready'))

func network_ready(_response):
	if g.auth_uuid != null:
		g.network.authenticate(g.auth_uuid, g.auth_pwd, funcref(self, 'authenticated'))
	else:
		g.auth_pwd = ''
		for _i in range(24):
			g.auth_pwd += char(randi() % 26 + 0x61) # Random lowercase letter
		print(g.auth_pwd)
		g.network.register_new_account(g.auth_pwd, funcref(self, 'account_created'))
	
func account_created(response):
	print('account created')
	print(response)
	g.auth_uuid = response.uuid
	g.save()
	after_authenticated(response.user_name, response.tag)

func authenticated(response):
	# TODO: jim: handle failure
	print('authenticated')
	print(response)
	after_authenticated(response.user_name, response.tag)

func after_authenticated(user_name, tag):
	online_status.text = 'Online as %s [%s]' % [user_name, tag]
	placeholder_button.connect('pressed', self, 'test_network')

func test_network():
	g.network.floop(funcref(self, 'floop_done'))

func floop_done(response):
	print('Floop response received.')
	print(response)

func on_play_button_pressed():
	if g.first_play:
		g.first_play = false
		g.save()
		if yield(g.summon_confirm('It looks like this is your first time playing. Play the tutorial?'), 'popup_closed') == true:
			on_tutorial_button_pressed()
			return
	g.backend = RandomAiBackend.new(g)
	print(g.backend.rules)
	g.scene_loader.goto_scene('game_book_select')

func on_tutorial_button_pressed():
	g.backend = TutorialBackend.new(g)
	var tutorial_overlay = TutorialOverlay.instance()
	get_node('/root').add_child(tutorial_overlay)
	tutorial_overlay.play_tutorial()
	print(g.backend.rules)
	g.scene_loader.goto_scene('game_book_select')
