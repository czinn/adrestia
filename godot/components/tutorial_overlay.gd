extends Control

signal node_found(node)
signal node_acquired(node)
signal popup_closed()

onready var g = get_node('/root/global')

onready var mouse_blocker = $mouse_blocker
onready var big_text_wnd = $nine_patch_rect
onready var tree_poll_timer = $tree_poll_timer
onready var big_text = $nine_patch_rect/margin_container/rich_text_label

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
	if node != null:
		emit_signal('node_acquired', node)
	desire_path = path
	node = yield(self, 'node_found')
	desire_path = null
	emit_signal('node_acquired', node)

func show_big_window(text):
	big_text.bbcode_text = text
	mouse_blocker.visible = true
	big_text_wnd.visible = true
	yield(self, 'popup_closed')

func show_tooltip(target, text):
	g.summon_tooltip(target, text)
	yield(g, 'tooltip_closed')

func play_tutorial():
	self.acquire_node('ui/selected_books_hbox')
	var selected_books_hbox = yield(self, 'node_acquired')
	yield(show_big_window('[b]Hello there![/b]\n\nWelcome to Adrestia!'), 'completed')
	yield(show_big_window('[b]Hi[/b]\n\nThis is the second window.'), 'completed')
	self.acquire_node('ui/books_scroll/books_hbox')
	var books_hbox = yield(self, 'node_acquired')
	yield(show_tooltip(books_hbox.get_child(0), 'This is a book. It has spells in it.'), 'completed')
	print('TODO: jim: continue the tutorial')
	self.get_parent().remove_child(self)
