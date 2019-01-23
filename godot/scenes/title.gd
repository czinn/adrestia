extends Node

onready var g = get_node('/root/global')
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
	placeholder_button.connect('pressed', self, 'test_network')
	if not g.loaded:
		g.loaded = true
		animation_player.play('fade_in')
		yield(animation_player, 'animation_finished')
	get_tree().set_auto_accept_quit(true)

func test_network():
	g.network.floop(funcref(self, 'floop_done_1'))

func floop_done_1(response):
	print('Floop is done!')
	print(response)
	g.network.floop(funcref(self, 'floop_done_2'))

func floop_done_2(response):
	print('Other floop is done!')
	print(response)

func on_play_button_pressed():
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
