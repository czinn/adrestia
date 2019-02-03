extends Node

const TutorialOverlay = preload('res://components/tutorial_overlay.tscn')
const RandomAiBackend = preload('res://backends/random_ai.gd')
const TutorialBackend = preload('res://backends/tutorial.gd')
const OnlineBackend = preload('res://backends/online.gd')

onready var g = get_node('/root/global')
onready var button_multiplayer = $ui/button_multiplayer
onready var button_ai = $ui/button_ai
onready var button_tutorial = $ui/button_tutorial
onready var back_button = $ui/back_button

func _ready():
	button_multiplayer.connect('pressed', self, 'on_button_multiplayer_pressed')
	button_ai.connect('pressed', self, 'on_button_ai_pressed')
	button_tutorial.connect('pressed', self, 'on_button_tutorial_pressed')
	back_button.connect('pressed', self, 'on_back_button_pressed')
	g.network.register_handlers(self, 'on_connected', 'on_disconnected')

func on_connected():
	button_multiplayer.material = null

func on_disconnected():
	button_multiplayer.material = load('res://shaders/greyscale.material')

func _notification(what):
	if what == MainLoop.NOTIFICATION_WM_QUIT_REQUEST:
		self.call_deferred('on_back_button_pressed')

func on_button_multiplayer_pressed():
	if g.network.status == g.network.ONLINE:
		g.backend = OnlineBackend.new(g)
		g.scene_loader.goto_scene('game_book_select')
	else:
		print('not online')

func on_button_ai_pressed():
	g.backend = RandomAiBackend.new(g)
	g.scene_loader.goto_scene('game_book_select')

func on_button_tutorial_pressed():
	g.backend = TutorialBackend.new(g)
	var tutorial_overlay = TutorialOverlay.instance()
	get_node('/root').add_child(tutorial_overlay)
	tutorial_overlay.play_tutorial()
	g.scene_loader.goto_scene('game_book_select')

func on_back_button_pressed():
	g.scene_loader.goto_scene('title', true)
