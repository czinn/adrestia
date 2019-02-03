extends Node

onready var g = get_node('/root/global')
onready var online_status = $ui/online_status
onready var play_button = $ui/play_button
onready var placeholder_button = $ui/placeholder_button
onready var animation_player = $animation_player

const TutorialOverlay = preload('res://components/tutorial_overlay.tscn')
const TutorialBackend = preload('res://backends/tutorial.gd')

func _ready():
	if not g.loaded:
		g.loaded = true
		initialize()
		animation_player.play('fade_in')
		yield(animation_player, 'animation_finished')
	play_button.connect('pressed', self, 'on_play_button_pressed')
	placeholder_button.connect('pressed', self, 'test_network')
	g.network.register_handlers(self, 'on_connected', 'on_disconnected')

func initialize():
	get_tree().set_auto_accept_quit(true)
	get_tree().set_quit_on_go_back(true)
	g.load()
	print('User data dir is %s' % [OS.get_user_data_dir()])

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
