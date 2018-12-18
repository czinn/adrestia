extends Node

onready var g = get_node('/root/global')

onready var book_button_scene = preload('res://components/book_button.tscn')
onready var spell_list_scene = preload('res://components/spell_list.tscn')

onready var book_select = $ui
onready var book_grid = $ui/scroll_container/book_grid
onready var spell_list_container = $ui/spell_list_container
onready var play_button = $ui/play_button

var spell_lists = {}

func _ready():
	g.clear_children(book_grid)
	for book in g.rules.get_books().values():
		var book_button = book_button_scene.instance()
		book_grid.add_child(book_button)
		book_button.book = book
		book_button.connect('pressed', self, 'toggle_book', [book_button])
	play_button.connect('pressed', self, 'on_play_button_pressed')

func get_selected_books():
	var result = []
	for book_button in book_grid.get_children():
		if book_button.checked:
			result.append(book_button.book)
	return result

func toggle_book(book_button):
	var book_id = book_button.book.get_id()
	var already_checked = book_button.checked
	if already_checked or len(get_selected_books()) < 3:
		book_button.checked = not already_checked
		if book_button.checked:
			var spell_list = spell_list_scene.instance()
			spell_list.show_stats = true
			spell_list.spells = book_button.book.get_spells()
			spell_list_container.add_child(spell_list)
			spell_lists[book_id] = spell_list
		else:
			# Remove the corresponding spell list
			var spell_list = spell_lists[book_id]
			spell_list.get_parent().remove_child(spell_list)
			spell_lists.erase(book_id)

func on_play_button_pressed():
	var selected_books = get_selected_books()
	if len(selected_books) < 1 or len(selected_books) > 3:
		return
	var selected_book_ids = g.map_method(selected_books, 'get_id')
	g.state = g.GameState.new()
	g.state.init(g.rules, [selected_book_ids, selected_book_ids])
	g.ai = g.Strategy.new()
	g.ai.init_cfr_strategy()
	g.scene_loader.goto_scene('game')
