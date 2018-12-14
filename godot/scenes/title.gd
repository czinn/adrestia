extends Node

onready var g = get_node('/root/global')
onready var play_button = $ui/play_button
onready var animation_player = $animation_player

func _ready():
	print('Unique ID is:')
	print(OS.get_unique_id())
	get_tree().set_auto_accept_quit(true)
	get_tree().set_quit_on_go_back(true)
	play_button.connect('pressed', self, 'on_play_button_pressed')
	# jim: We definitely need an official waifu at some point, but Moge-ko isn't it.
	#waifu.connect('pressed', self, 'on_waifu_pressed')
	if not g.loaded:
		g.loaded = true
		animation_player.play('fade_in')
		yield(animation_player, 'animation_finished')

func on_waifu_pressed():
	g.summon_tooltip($ui/moge_ko, "Hey now, why are you runniiing? No, no, I wanna play!\n... [i]Oooh, I see, it's tag![/i]\n[b]GYAHAHAHAHAHAA!!!![/b]")

func on_play_button_pressed():
	g.scene_loader.goto_scene('game_book_select')
