extends Control

onready var animation_player = $animation_player
onready var texture_rect = $texture_rect

const margin = 40

func on_resize():
	var window_size = get_viewport_rect().size
	var texture_size = texture_rect.texture.get_size()

	# Desired dimensions of the TextureRect.
	var h = window_size.y + margin * 2
	var w = h * texture_size.x / texture_size.y

	var x0 = window_size.x + margin
	var dx = window_size.x / 2 + w / 2 + 2 * margin
	var y = -margin

	texture_rect.margin_top = y
	texture_rect.margin_bottom = y + h
	texture_rect.margin_left = x0
	texture_rect.margin_right = x0 + w
	texture_rect.rect_pivot_offset = texture_rect.rect_size / 2

	var pos_0 = Vector2(x0, y)
	var pos_1 = Vector2(x0 - dx, y)
	var pos_2 = Vector2(x0 - 2 * dx, y)

	var slide_in = animation_player.get_animation('slide_in')
	slide_in.track_set_key_value(0, 0, pos_0)
	slide_in.track_set_key_value(0, 1, pos_1)

	var slide_out = animation_player.get_animation('slide_out')
	slide_out.track_set_key_value(0, 0, pos_1)
	slide_out.track_set_key_value(0, 1, pos_2)

func _ready():
	self.on_resize()
