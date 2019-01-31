extends Node

onready var g = get_node('/root/global')
onready var online_status = $ui/online_status
onready var play_button = $ui/play_button
onready var placeholder_button = $ui/placeholder_button
onready var animation_player = $animation_player

const TutorialOverlay = preload('res://components/tutorial_overlay.tscn')
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
	placeholder_button.connect('pressed', self, 'test_network')
	if not g.loaded:
		g.loaded = true
		initialize()
		animation_player.play('fade_in')
		yield(animation_player, 'animation_finished')
	get_tree().set_auto_accept_quit(true)

func initialize():
	g.load()
	print(OS.get_user_data_dir())
	g.network.connect('connected', self, 'on_connected')
	g.network.connect('disconnected', self, 'on_disconnected')

func on_connected():
	online_status.text = 'Online as %s [%s]' % [g.user_name, g.tag]

func on_disconnected():
	online_status.text = 'Offline.'
	
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
	g.scene_loader.goto_scene('game_mode_select')

func on_tutorial_button_pressed():
	g.backend = TutorialBackend.new(g)
	var tutorial_overlay = TutorialOverlay.instance()
	get_node('/root').add_child(tutorial_overlay)
	tutorial_overlay.play_tutorial()
	g.scene_loader.goto_scene('game_book_select')
