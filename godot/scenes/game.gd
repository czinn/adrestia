extends Node

# Book Select.
# (Maybe this should be a separate scene that we instantiate.

onready var g = get_node('/root/global')
onready var book_select = $ui/book_select
onready var book_grid = $ui/book_select/book_grid
onready var play_button = $ui/book_select/play_button

onready var book_button_scene = preload('res://components/book_button.tscn')
onready var spell_list_scene = preload('res://components/spell_list.tscn')
onready var spell_button_scene = preload('res://components/spell_button.tscn')

# The game.
onready var spell_select = $ui/spell_select
onready var end_turn_button = $ui/spell_select/end_turn_button
onready var book_tabs = $ui/spell_select/book_tabs
onready var player_stats = $ui/player_stats
onready var spell_queue = $ui/spell_queue

func _ready():
	print('ain nobody bring us')
	g.clear_children(book_grid)
	spell_queue.spells = []
	for book in g.rules.get_books().values():
		var book_button = book_button_scene.instance()
		book_grid.add_child(book_button)
		book_button.book = book
		book_button.connect('pressed', self, 'toggle_book', [book_button])
	play_button.connect('pressed', self, 'on_play_button_pressed')
	end_turn_button.connect('pressed', self, 'on_end_turn_button_pressed')
	spell_queue.connect('pressed', self, 'on_spell_queue_pressed')
	book_select.visible = true
	spell_select.visible = false
	player_stats.visible = false
	spell_queue.visible = false

func get_selected_books():
	var result = []
	for book_button in book_grid.get_children():
		if book_button.checked:
			result.append(book_button.book)
	return result

func toggle_book(book_button):
	var already_checked = book_button.checked
	if already_checked or len(get_selected_books()) < 3:
		book_button.checked = not already_checked

func on_play_button_pressed():
	var selected_books = get_selected_books()
	if len(selected_books) < 1 or len(selected_books) > 3:
		return
	var selected_book_ids = g.map_method(selected_books, 'get_id')
	g.state = g.GameState.new()
	g.state.init(g.rules, [selected_book_ids, ['conjuration']])
	book_select.visible = false
	spell_select.visible = true
	player_stats.visible = true
	spell_queue.visible = true
	print(g.state.as_json().result)

	g.clear_children(book_tabs)
	
	selected_books = g.state.players[0].books # Not needed but just in case

	# Populate spell list
	for i in range(len(selected_books)):
		var book = selected_books[i]
		var tab = NinePatchRect.new()
		tab.name = book.get_id()
		var spell_list = spell_list_scene.instance()
		spell_list.spells = book.get_spells()
		spell_list.connect('pressed', self, 'on_spell_enqueue')
		tab.add_child(spell_list)
		book_tabs.add_child(tab)
	redraw()

func on_spell_enqueue(index, spell):
	var action = spell_queue.spells.duplicate()
	action.append(spell.get_id())
	if not g.state.is_valid_action(0, action):
		return
	spell_queue.spells = action
	redraw()

func on_spell_queue_pressed(index, spell):
	var action = spell_queue.spells.duplicate()
	action.remove(index)
	if not g.state.is_valid_action(0, action):
		return
	spell_queue.spells = action
	redraw()

onready var hp_icon = $ui/player_stats/hp_icon
onready var mp_icon = $ui/player_stats/mp_icon
onready var hp_label = $ui/player_stats/hp_label
onready var mp_label = $ui/player_stats/mp_label

func redraw():
	var me = g.state.players[0]
	var mp_left = me.mp
	for spell_id in spell_queue.spells:
		var spell = g.rules.get_spell(spell_id)
		mp_left -= spell.get_cost()
	hp_label.text = '%d/%d' % [me.hp, me.max_hp]
	mp_label.text = '%d/%d (+%d)' % [mp_left, g.rules.get_mana_cap(), me.mp_regen]

func on_end_turn_button_pressed():
	var action = spell_queue.spells
	if not g.state.is_valid_action(0, action):
		return
	
	# Enemy does nothing
	# TODO: AIs
	var enemy_action = []

	g.state.simulate([action, enemy_action])
	spell_queue.spells = []
	redraw()
