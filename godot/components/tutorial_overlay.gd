extends Control

signal node_found(node)
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
	if path != '':
		node = node.get_node(path);
		if node == null: return
		return node
	else:
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
	yield(get_tree(), 'idle_frame')
	if node != null:
		print('Got node immediately')
		return node
	desire_path = path
	print('Waiting for node...')
	node = yield(self, 'node_found')
	print('Found it')
	desire_path = null
	return node

func show_big_window(text):
	big_text.bbcode_text = text
	mouse_blocker.visible = true
	big_text_wnd.visible = true
	yield(self, 'popup_closed')

func show_tooltip(target, text):
	g.summon_tooltip(target, text)
	yield(g, 'tooltip_closed')

func play_tutorial():
	var selected_books_hbox = yield(self.acquire_node('ui/selected_books_hbox'), 'completed')
	yield(show_big_window('[b]Welcome to Adrestia![/b]\n\nIn Adrestia, players cast spells in order to reduce their opponent\'s health to zero. The first player to do so wins!'), 'completed')
	var books_hbox = yield(self.acquire_node('ui/books_scroll/books_hbox'), 'completed')
	yield(show_tooltip(books_hbox.get_child(0), 'This is a book. Each book contains four spells.\n\nEach player secretly chooses three books at the beginning of the game.'), 'completed')
	yield(show_big_window('Choose a book by dragging it to one of the slots at the top.'), 'completed')
	var root = yield(self.acquire_node(''), 'completed')
	var book = yield(root, 'chose_book')[1]
	yield(show_big_window('The number in the blue icon shows how much each spell costs.\n\nYou can tap and hold a spell to see what it does.'), 'completed')
	yield(show_big_window('The green book in the corner of each spell shows how much knowledge is required to cast the spell.\n\nYou can get up to one knowledge per turn in the book of your choice.'), 'completed')
	yield(show_big_window('Finish choosing three books and tap the [b]Play[/b] button.'), 'completed')
	var spell_select = yield(self.acquire_node('ui/spell_select'), 'completed')
	yield(show_big_window('Nice work! Let\'s take a look around the game screen.'), 'completed')
	var my_stats = yield(self.acquire_node('ui/my_stats'), 'completed')
	yield(show_tooltip(my_stats, 'Here is your remaining health and mana. The (+5) beside your mana shows you how much mana you get each turn. You can increase this with spells.'), 'completed')
	yield(show_tooltip(spell_select, 'Here are your books. You can tap a book to see its spells.'), 'completed')
	yield(show_tooltip(spell_select, 'The number on each book is how much knowledge you have of that book. You can increase your knowledge of a book by tapping the green arrow on the book.'), 'completed')
	print('TODO: jim: continue the tutorial')
	self.get_parent().remove_child(self)
