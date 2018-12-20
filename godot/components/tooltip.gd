extends Control

onready var g = get_node('/root/global')

onready var background = $background
onready var ninepatch = $background/nine_patch_rect
onready var label = $background/nine_patch_rect/rich_text_label
onready var triangle = $background/triangle

var text = null setget set_text
var x = 0
var y = 0
var point_down = true

# reasoning: triangle height is 20
# + 5 for border thickness
# + 2 for margin of error
const triangle_shift = 27

# TODO: jim: autodetect this
const line_height = 26

func _ready():
	redraw()

func _gui_event(event):
	if g.event_is_pressed(event):
		g.close_tooltip()

func set_text(text_):
	text = text_
	redraw()

func set_target(x_, y_, point_down_=true):
	x = x_
	y = y_
	point_down = point_down_
	redraw()

func redraw():
	if triangle == null: return
	if label == null: return

	label.bbcode_text = text
	var margin = label.margin_top + ninepatch.margin_top + 2

	var triangle_size = triangle.rect_size
	triangle.margin_left = x - floor(triangle_size.x / 2)
	triangle.margin_right = triangle.margin_left + triangle_size.x
	if point_down:
		triangle.anchor_top = 1
		triangle.anchor_bottom = 1
		triangle.margin_top = -triangle_shift
		triangle.rect_scale.y = 1
	else:
		triangle.anchor_top = 0
		triangle.anchor_bottom = 0
		triangle.margin_top = triangle_shift
		triangle.rect_scale.y = -1
	triangle.margin_bottom = triangle.margin_top + triangle_size.y

	# XTODO: charles: This doesn't work if any lines wrap. We want to use
	# get_visible_line_count() instead, but there's a bug where it always returns
	# 0, so we cant.
	# jim: Looks like we have to wait a frame so that the RichTextLabel reflows
	# its text. ... though in addition to that, it seems that [get_line_count]
	# ONLY counts lines from newline characters, whereas [get_visible_line_count]
	# ONLY counts lines from wrapping... so here we go.
	# This will probably break when Godot is fixed.

	# Try to set tooltip text for three frames, seeing if # of computed lines
	# changes at each frame.
	var old_lines = null
	for _unused in range(3):
		var lines = label.get_line_count() + max(label.get_visible_line_count(), 1) - 1
		if lines == old_lines:
			break
		old_lines = lines
		var height = lines * line_height + margin * 2
		if point_down:
			background.margin_top = y - height
			background.margin_bottom = y
		else:
			background.margin_top = y
			background.margin_bottom = y + height
		get_tree() && yield(get_tree(), 'idle_frame')
