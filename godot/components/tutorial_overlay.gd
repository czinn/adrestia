extends Control

signal node_found(node)
signal popup_closed()

onready var g = get_node('/root/global')

onready var mouse_blocker = $mouse_blocker
onready var big_text_wnd = $nine_patch_rect
onready var tree_poll_timer = $tree_poll_timer

func _ready():
	mouse_blocker.connect('gui_input', self, 'blocker_input')
	tree_poll_timer.connect('timeout', self, 'timeout')
	big_text_wnd.visible = false
	mouse_blocker.visible = false

func blocker_input(event):
	if g.event_is_pressed(event):
		big_text_wnd.visible = false
		mouse_blocker.visible = false
		emit_signal('popup_closed')

func try_get_node(path):
	var node = get_node('/root/scene_loader')
	if node == null: return
	node = node.get_child(0);
	if node == null: return
	node = node.get_child(0);
	if node == null: return
	node = node.get_node(path);
	if node == null: return
	return node

var desire_path = null
func timeout():
	if desire_path == null: return
	var node = try_get_node(desire_path)
	if node == null: return
	# jim: We don't set desire_path back to null here just in case and a timeout
	# happens such that the yield misses the emit. (are godot classes monitors?)
	emit_signal('node_found', node)

func acquire_node(path):
	var node = try_get_node(path)
	if node != null: return node
	desire_path = path
	node = yield(self, 'node_found')
	desire_path = null
	return node

func play_tutorial():
	yield(self.acquire_node('ui/selected_books_hbox'), 'completed')
	mouse_blocker.visible = true
	big_text_wnd.visible = true
	yield(self, 'popup_closed')
	print('TODO: jim: continue the tutorial')
