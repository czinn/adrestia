extends Node

onready var g = get_node('/root/global')

onready var spell_list_scene = preload('res://components/spell_list.tscn')

onready var game = $ui
onready var spell_select = $ui/spell_select
onready var end_turn_button = $ui/spell_select/end_turn_button
onready var book_tabs = $ui/spell_select/book_tabs
onready var spell_queue = $ui/spell_queue
onready var player_stats = $ui/player_stats
onready var enemy_stats = $ui/enemy_stats

func _ready():
	end_turn_button.connect('pressed', self, 'on_end_turn_button_pressed')
	spell_queue.connect('pressed', self, 'on_spell_queue_pressed')
	spell_queue.spells = []
	spell_queue.show_stats = false

	# Populate spell lists
	g.clear_children(book_tabs)
	var selected_books = g.state.players[0].books
	for i in range(len(selected_books)):
		var book = selected_books[i]
		var spell_list = spell_list_scene.instance()
		spell_list.show_stats = true
		spell_list.enabled_filter = funcref(self, 'player_can_cast')
		spell_list.unlocked_filter = funcref(self, 'player_has_unlocked_spell')
		# Set the spell list last so that we don't redraw so many times
		spell_list.spells = book.get_spells()
		spell_list.connect('pressed', self, 'on_spell_enqueue')
		book_tabs.add_child(spell_list)

	# allow tab container to detect new tabs...
	yield(get_tree(), 'idle_frame')
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

# TODO: jim: All the player_* functions duplicate some part of our game logic.
# Move these to helper functions in C++ and wrap them?
func player_upgraded_book():
	for spell_id in spell_queue.spells:
		var spell = g.rules.get_spell(spell_id)
		if spell.is_tech_spell():
			return spell.get_book()
	return null

# TODO: jim: This is O(n) in the number of spells in the queue. Not super
# important to improve, but feels bad.
func player_mp_left():
	var me = g.state.players[0]
	var mp_left = me.mp
	for spell_id in spell_queue.spells:
		var spell = g.rules.get_spell(spell_id)
		mp_left -= spell.get_cost()
	return mp_left

func player_can_afford(spell):
	if spell.is_tech_spell() and player_upgraded_book() != null:
		return false
	var me = g.state.players[0]
	var mp_left = player_mp_left()
	return spell.get_cost() <= mp_left

# TODO: jim: This is ugly because we have to zip through Player.tech and
# Player.books. Make it not ugly.
func player_effective_tech(book_id):
	var me = g.state.players[0]
	for i in range(len(me.tech)):
		if me.books[i].get_id() == book_id:
			return me.tech[i] + (1 if player_upgraded_book() == book_id else 0)
	return 0

func player_effective_level():
	var me = g.state.players[0]
	return g.sum(me.tech) + (1 if player_upgraded_book() != null else 0)

func player_has_unlocked_spell(spell):
	return (player_effective_level() >= spell.get_level() and
			player_effective_tech(spell.get_book()) >= spell.get_tech())

func player_can_cast(spell):
	if spell.is_tech_spell() and player_upgraded_book() != null:
		return false
	return player_has_unlocked_spell(spell) && player_can_afford(spell)

func redraw():
	var me = g.state.players[0]
	var mp_left = player_mp_left()
	enemy_stats.redraw(g.state.players[1])
	player_stats.redraw(me, mp_left)

	# display effective tech levels
	for i in range(len(me.tech)):
		var book_id = me.books[i].get_id()
		book_tabs.set_tab_title(i, '%s (%d)' % [book_id, player_effective_tech(book_id)])
	book_tabs.update()

	for spell_list in book_tabs.get_children():
		spell_list.redraw()
	spell_queue.redraw()

func on_end_turn_button_pressed():
	var action = spell_queue.spells
	if not g.state.is_valid_action(0, action):
		return
	
	var view = g.GameView.new()
	view.init(g.state, 1)
	var enemy_action = g.ai.get_action(view)
	print(enemy_action)

	g.state.simulate([action, enemy_action])
	spell_queue.spells = []
	redraw()

	if len(g.state.winners()) > 0:
		print('Game is over')
		g.scene_loader.goto_scene('game_results')
