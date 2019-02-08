extends Control

signal finished

onready var timer = $timer
onready var label = $label

var seconds = 60 setget set_seconds

func _ready():
	timer.connect('timeout', self, 'on_tick')

func set_seconds(seconds_):
	seconds = seconds_
	redraw()
	timer.stop()
	timer.start()

func on_tick():
	if seconds > 0:
		seconds -= 1
		redraw()
		if seconds == 0:
			emit_signal('finished')

func redraw():
	label.text = str(seconds)