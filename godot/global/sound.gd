extends Node

const transition_time = 0.5
const same_sound_debounce_ms = 100

var music_player
var current_filename
var sound_last_played = {}

onready var g = get_node('/root/global')

func _ready():
	music_player = AudioStreamPlayer.new()
	add_child(music_player)

func set_music(music_name):
	if current_filename != music_name:
		current_filename = music_name
		var tween = Tween.new()
		add_child(tween)
		tween.interpolate_method(
			music_player,
			'set_volume_db',
			music_player.volume_db,
			music_player.volume_db - 30,
			transition_time,
			Tween.TRANS_LINEAR,
			Tween.EASE_IN)
		tween.start()
		yield(tween, 'tween_completed')
		tween.queue_free()
		music_player.stop()
		var stream = load('res://sound/%s.ogg' % [music_name])
		stream.loop = true
		music_player.stream = stream
		if music_name == 'title': music_player.volume_db = -3.0
		elif music_name == 'battle': music_player.volume_db = -2.0
		else: music_player.volume_db = 0.0
		music_player.play()

func play_sound(sound_name):
	if sound_last_played.has(sound_name) and OS.get_ticks_msec() - sound_last_played[sound_name] < same_sound_debounce_ms:
		return
	sound_last_played[sound_name] = OS.get_ticks_msec()
	var sound_player = AudioStreamPlayer.new()
	add_child(sound_player)
	sound_player.stream = load('res://sound/%s.wav' % [sound_name])
	sound_player.play()
	yield(sound_player, 'finished')
	sound_player.queue_free()
