extends Control

onready var g = get_node('/root/global')

onready var duration_label = $duration_label

func _ready():
	pass

func redraw(sticky):
	var duration = sticky.remaining_duration
	var unit = duration.get_unit()
	match unit:
		g.DurationUnit.FULL_GAME:
			duration_label.text = ''
		g.DurationUnit.TURNS:
			duration_label.text = str(duration.get_value())
		g.DurationUnit.STEPS:
			duration_label.text = '0.' + str(duration.get_value())
