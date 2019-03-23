extends Node

const transition_time = 0.5

var audio_player
var current_filename
onready var g = get_node('/root/global')

func _ready():
	audio_player = AudioStreamPlayer.new()
	print(audio_player.volume_db)
	add_child(audio_player)

func set_music(filename):
	if current_filename != filename:
		current_filename = filename
		var tween = Tween.new()
		add_child(tween)
		tween.interpolate_method(audio_player, 'set_volume_db', 0, -30, transition_time, Tween.TRANS_LINEAR, Tween.EASE_IN)
		tween.start()
		yield(tween, 'tween_completed')
		tween.queue_free()
		audio_player.stop()
		var stream = load('res://sound/%s.ogg' % [filename])
		stream.loop = true
		audio_player.stream = stream
		audio_player.volume_db = 0.0
		audio_player.play()
