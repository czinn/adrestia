extends Node

onready var g = get_node('/root/global')

onready var book_button_scene = preload('res://components/book_button.tscn')
onready var spell_button_list_scene = preload('res://components/spell_button_list.tscn')
onready var book_placeholder_texture = preload('res://art-built/book-select-empty.png')
onready var Tweener = preload('res://global/tweener.gd')

onready var ui = $ui
onready var spells_panel = $ui/panel
onready var books_hbox = $ui/books_scroll/books_hbox
onready var spell_list_container = $ui/spell_list_container
onready var play_button = $ui/play_button
onready var selected_books_hbox = $ui/selected_books_hbox

var chosen_books = [null, null, null]
var book_buttons = {}

func _ready():
	g.clear_children(books_hbox)
	for book in g.rules.get_books().values():
		var book_button = book_button_scene.instance()
		books_hbox.add_child(book_button)
		book_button.book = book
		book_button.button.connect('pressed', self, 'on_press_book', [book_button])
		book_button.button.connect('button_down', self, 'on_book_down', [book_button])
		book_buttons[book.get_id()] = book_button
	play_button.connect('pressed', self, 'on_play_button_pressed')
	for i in range(selected_books_hbox.get_child_count()):
		var button = selected_books_hbox.get_child(i)
		button.connect('pressed', self, 'on_remove_book', [i, button])
		button.connect('button_down', self, 'on_remove_book_down', [i, button])

func show_book_detail(book):
	# set spell list
	var spell_button_list = spell_button_list_scene.instance()
	spell_button_list.show_stats = true
	spell_button_list.spells = book.get_spells()
	g.clear_children(spell_list_container)
	spell_list_container.add_child(spell_button_list)

func on_lift():
	print('lift')
	g.drag_drop.payload.button.texture_normal = book_placeholder_texture

func on_drop(drag_image):
	var button = g.drag_drop.payload
	var book = button.book
	var i = chosen_books.find(null)
	if g.drag_drop.drag_end.y > spells_panel.get_global_position().y or chosen_books.find(book) >= 0 or i == -1:
		yield(g.tween(drag_image, button.button.get_global_position(), 0.3), 'done')
		button.button.texture_normal = drag_image.texture
		drag_image.queue_free()
		return
	chosen_books[i] = book
	var chosen_book_icon = selected_books_hbox.get_child(i)
	yield(g.tween(drag_image, chosen_book_icon.get_global_position(), 0.3), 'done')
	drag_image.queue_free()
	selected_books_hbox.get_child(i).texture_normal = g.get_book_texture(book.get_id())

func on_book_down(book_button):
	show_book_detail(book_button.book)
	g.drag_drop.set_dead_zone(-20, null, null, null)
	g.drag_drop.on_lift = funcref(self, 'on_lift')
	g.drag_drop.on_drop = funcref(self, 'on_drop')
	g.drag_drop.payload = book_button
	g.drag_drop.track_drag(book_button.button)

func on_press_book(book_button):
	var book = book_button.book
	var i = chosen_books.find(null)
	if i == -1 or chosen_books.find(book) >= 0:
		return
	chosen_books[i] = book

	# animate book going to slot
	var image = g.drag_drop.clone_image(book_button.button)
	book_button.button.texture_normal = book_placeholder_texture
	var chosen_book_icon = selected_books_hbox.get_child(i)
	yield(g.tween(image, chosen_book_icon.get_global_position(), 0.3), 'done')
	image.queue_free()
	selected_books_hbox.get_child(i).texture_normal = g.get_book_texture(book.get_id())

func on_remove_book(i, button):
	if chosen_books[i] == null: return
	var book_id = chosen_books[i].get_id()
	chosen_books[i] = null

	# animate book going back to list
	var book_button = book_buttons[book_id]
	var image = g.drag_drop.clone_image(button)
	button.texture_normal = book_placeholder_texture
	yield(g.tween(image, book_button.get_global_position(), 0.3), 'done')
	image.queue_free()
	book_button.button.texture_normal = g.get_book_texture(book_id)

	# clear spell list
	g.clear_children(spell_list_container)

func on_remove_book_down(i, button):
	if chosen_books[i] == null: return
	g.drag_drop.set_dead_zone(0, 0, 0, 0)
	g.drag_drop.on_lift = funcref(self, 'on_remove_book_down_lift')
	g.drag_drop.on_drop = funcref(self, 'on_remove_book_down_drop')
	g.drag_drop.payload = [i, button]
	g.drag_drop.track_drag(button)

func on_remove_book_down_lift():
	match g.drag_drop.payload:
		[var i, var button]:
			button.texture_normal = book_placeholder_texture

func on_remove_book_down_drop(image):
	match g.drag_drop.payload:
		[var i, var button]:
			var book_id = chosen_books[i].get_id()
			var book_button = book_buttons[book_id]
			chosen_books[i] = null
			yield(g.tween(image, book_button.get_global_position(), 0.3), 'done')
			book_button.button.texture_normal = g.get_book_texture(book_id)
			image.queue_free()

func on_play_button_pressed():
	var selected_books = []
	for book in chosen_books:
		if book != null:
			selected_books.append(book)
	if len(selected_books) < 1 or len(selected_books) > 3:
		return
	var selected_book_ids = g.map_method(selected_books, 'get_id')
	g.state = g.GameState.new()
	g.state.init(g.rules, [selected_book_ids, ['conjuration']])
	g.ai = g.Strategy.new()
	g.ai.init_random_strategy()
	g.scene_loader.goto_scene('game')
