extends Node
# Extraordinarily flexible drag-n-drop.

onready var root = get_node('/root')
var drag_start = null
var payload = null
var drag_image = null
var dragged_node_start = null
var dragged_node_end = null

func _ready():
	set_process(false)

func _process(delta):
	if drag_image:
		var pos = get_viewport().get_mouse_position()
		var delta_pos = pos - drag_start
		drag_image.rect_position = dragged_node_start + delta_pos
	
func clone_image(node):
	var image = TextureRect.new()
	image.expand = true
	image.stretch_mode = TextureRect.STRETCH_KEEP_ASPECT_CENTERED
	if node is TextureButton:
		image.texture = node.texture_normal
		image.rect_size = node.rect_size
		image.rect_position = node.get_global_position()
	else:
		print('Warning: Attempted to clone unsupported type %s' % node.get_type())
		return
	root.add_child(image)
	return image

func start_drag(node, payload_):
	end_drag()
	payload = payload_
	drag_image = clone_image(node)
	drag_start = get_viewport().get_mouse_position()
	dragged_node_start = node.get_global_position()
	set_process(true)

func end_drag():
	var old_drag_image = drag_image
	if drag_image:
		dragged_node_end = get_viewport().get_mouse_position()
		drag_image = null
		set_process(false)
	return old_drag_image
