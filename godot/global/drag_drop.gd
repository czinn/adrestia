extends Node
# Extraordinarily flexible drag-n-drop.

enum {
	# Drag/drop is not active.
	STATE_IDLE,
	# Tracking user movements on a draggable object, but the object has not yet been lifted.
	STATE_TRACKING,
	# User has draggable object held down and is actively moving it around.
	STATE_DRAGGING,
	# Draggable object has snapped to a target.
	#STATE_SNAPPED,
}

onready var root = get_node('/root')
var state = STATE_IDLE
var drag_start = null
var drag_end = null
var payload = null
var dead_top = 0
var dead_left = 0
var dead_right = 0
var dead_bottom = 0
var tracked_node = null
var tracked_node_start = null
var drag_image = null
var drag_image_start = null
var on_lift = null # funcref()
var on_drop = null # funcref()

func _input(event):
	if event is InputEventMouseButton and event.button_index == BUTTON_LEFT and not event.pressed:
		match state:
			STATE_TRACKING:
				state = STATE_IDLE
			STATE_DRAGGING:
				var drag_image = self.drag_image
				end_drag()
				self.on_drop.call_func(drag_image)
				state = STATE_IDLE

func _ready():
	set_process(false)

func _process(delta):
	var pos = get_viewport().get_mouse_position()
	match state:
		STATE_IDLE:
			set_process(false)
		STATE_TRACKING:
			var tracked_rect = tracked_node.get_global_rect()
			tracked_rect = tracked_rect.grow_individual(self.dead_left, self.dead_top, self.dead_right, self.dead_bottom)
			if not tracked_rect.has_point(pos):
				state = STATE_DRAGGING
				self.drag_image = self.clone_image(tracked_node)
				self.drag_image_start = tracked_node.get_global_position()
				self.on_lift.call_func()
		STATE_DRAGGING:
			var delta_pos = pos - drag_start
			drag_image.rect_position = drag_image_start + delta_pos
	
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

func start_drag(node, payload):
	end_drag()
	self.payload = payload
	self.drag_image = clone_image(node)
	self.drag_start = get_viewport().get_mouse_position()
	self.drag_image_start = node.get_global_position()
	set_process(true)

# Tracks the user's mouse cursor, starting a drag (and triggering [on_lift])
# after a dead zone is exited.
func track_drag(node):
	end_drag()
	self.tracked_node = node
	self.tracked_node_start = node.get_global_position()
	self.drag_start = get_viewport().get_mouse_position()
	state = STATE_TRACKING
	set_process(true)

func end_drag():
	if self.drag_image:
		self.drag_end = get_viewport().get_mouse_position()
		self.drag_image = null
		set_process(false)

func set_dead_zone(top, left, right, bottom):
	if top == null: top = 9999
	if left == null: left = 9999
	if right == null: right = 9999
	if bottom == null: bottom = 9999
	self.dead_top = top
	self.dead_left = left
	self.dead_right = right
	self.dead_bottom = bottom
