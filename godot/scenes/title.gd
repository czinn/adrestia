extends Node

onready var g = get_node('/root/global')
onready var play_button = $ui/play_button
onready var waifu = $ui/moge_ko

func _ready():
	get_tree().set_auto_accept_quit(true)
	get_tree().set_quit_on_go_back(true)
	play_button.connect('pressed', self, 'on_play_button_pressed')
	waifu.connect('pressed', self, 'on_waifu_pressed')

func on_waifu_pressed():
	g.summon_tooltip($ui/moge_ko, "Hey now, why are you runniiing? No, no, I wanna play!\n... [i]Oooh, I see, it's tag![/i]\n[b]GYAHAHAHAHAHAA!!!![/b]")

func on_play_button_pressed():
	g.scene_loader.goto_scene('game')
