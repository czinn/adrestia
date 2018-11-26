extends Control

onready var g = get_node('/root/global')

onready var duration_label = $duration_label

var sticky = null setget set_sticky

func _ready():
	pass

func _gui_event(event):
	if g.event_is_pressed(event) && sticky != null:
		var t = "[b]" + sticky.sticky.get_name() + "[/b]\n" + sticky.sticky.get_text()
		print(t)
		g.summon_tooltip(self, t, true)

func set_sticky(sticky_):
	sticky = sticky_
	redraw()

func redraw():
	if g == null: return
	if sticky == null: return

	var duration = sticky.remaining_duration

	var unit = duration.get_unit()
	match unit:
		g.DurationUnit.FULL_GAME:
			duration_label.text = ''
		g.DurationUnit.TURNS:
			duration_label.text = str(duration.get_value())
		g.DurationUnit.STEPS:
			duration_label.text = '0.' + str(duration.get_value())
