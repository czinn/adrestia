extends Control

onready var background = $background
onready var label = $background/nine_patch_rect/rich_text_label
onready var triangle = $background/triangle

var text = null setget set_text
var x = 270
var y = 490
var point_down = true

const line_height = 28
const margin = 30 + 72 # aaahhh magic numbers

func _ready():
	redraw()

func _gui_event(event):
	if event is InputEventMouseButton and event.button_index == BUTTON_LEFT and event.pressed:
		get_parent().remove_child(self)

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
	# TODO: charles: This doesn't work if any lines wrap. We want to use
	# get_visible_line_count() instead, but there's a bug where it always returns
	# 0, so we cant.
	var height = label.get_line_count() * line_height + margin * 2 + 5
	
	triangle.margin_left = x - 40
	if point_down:
		triangle.anchor_top = 1
		triangle.anchor_bottom = 1
		triangle.margin_top = -78
		triangle.margin_bottom = 0
		triangle.rect_scale.y = 1
		background.margin_top = y - height
		background.margin_bottom = y
	else:
		triangle.rect_scale.y = -1
		triangle.anchor_top = 0
		triangle.anchor_bottom = 0
		triangle.margin_top = 78
		triangle.margin_bottom = 0
		background.margin_top = y
		background.margin_bottom = y + height
