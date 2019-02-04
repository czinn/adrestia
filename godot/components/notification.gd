extends Control

onready var g = get_node('/root/global')

onready var background = $mover/background
onready var ninepatch = $mover/background/nine_patch_rect
onready var label = $mover/background/nine_patch_rect/rich_text_label
onready var animation_player = $animation_player

var text = ''

const line_height = 26

var showing_notification = false
var future_notifications = []

func _ready():
	redraw()

func _gui_event(event):
	if g.event_is_pressed(event) && !animation_player.is_playing():
		animation_player.play_backwards('slide_in')
		animation_player.connect('animation_finished', self, 'show_next_notification')

func show_notification(message):
	if not showing_notification:
		showing_notification = true
		text = message
		redraw()
		animation_player.play('slide_in')
	else:
		future_notifications.append(message)

func show_next_notification(_animation_name):
	animation_player.disconnect('animation_finished', self, 'show_next_notification')
	if len(future_notifications) > 0:
		text = future_notifications.pop_front()
		redraw()
		animation_player.play('slide_in')
	else:
		showing_notification = false

func redraw():
	label.bbcode_text = text
	var margin = label.margin_top - label.margin_bottom + ninepatch.margin_top - ninepatch.margin_bottom + 2
	var old_lines = null
	for _unused in range(3):
		var lines = label.get_line_count() + max(label.get_visible_line_count(), 1) - 1
		if lines == old_lines:
			break
		old_lines = lines
		var height = lines * line_height + margin
		background.margin_bottom = height
		get_tree() && yield(get_tree(), 'idle_frame')
