extends Control

onready var g = get_node('/root/global')

onready var background = $mover/background
onready var ninepatch = $mover/background/nine_patch_rect
onready var label = $mover/background/nine_patch_rect/rich_text_label
onready var animation_player = $animation_player
onready var timer = $timer

const line_height = 26

# [sticky]: If true, stay around until dismissed. By default, notifications
# disappear by themselves after a bit.
# [on_click]: funcref that will be called when the sticky is dismissed, or
# null.
var showing_notification = false
var future_notifications = []
var text = null
var sticky = false
var on_click = null

func _ready():
	redraw()
	timer.connect('timeout', self, 'exit_notification')

func _gui_event(event):
	if g.event_is_pressed(event) && !animation_player.is_playing():
		exit_notification()

func exit_notification():
	if not showing_notification: return
	if on_click:
		on_click.call_func()
		on_click = null
	animation_player.play_backwards('slide_in')
	yield(animation_player, 'animation_finished')
	if len(future_notifications) > 0:
		callv('show_notification', future_notifications.pop_front())
	else:
		showing_notification = false

func show_notification(text_, sticky_, on_click_):
	showing_notification = true
	text = text_
	sticky = sticky_
	on_click = on_click_
	if not sticky:
		timer.start()
	redraw()
	animation_player.play('slide_in')

func push_notification(text_, sticky_=false, on_click_=null):
	if not showing_notification:
		showing_notification = true
		show_notification(text_, sticky_, on_click_)
	else:
		future_notifications.append([text_, sticky_, on_click_])

func redraw():
	if text == null: return

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
