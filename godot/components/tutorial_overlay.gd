extends Control

signal node_found(node)
signal popup_closed()

onready var g = get_node('/root/global')

onready var mouse_blocker = $mouse_blocker
onready var big_text_wnd = $nine_patch_rect
onready var tree_poll_timer = $tree_poll_timer
onready var big_text = $nine_patch_rect/margin_container/rich_text_label

var finished_first_turn = false
var open_time = 0

func _ready():
	mouse_blocker.connect('gui_input', self, 'blocker_input')
	tree_poll_timer.connect('timeout', self, 'timeout')
	big_text_wnd.visible = false
	mouse_blocker.visible = false

var force_click_rect = null
func blocker_input(event):
	if OS.get_ticks_msec() - open_time < 500:
		return
	if g.event_is_pressed(event):
		if force_click_rect != null && !force_click_rect.has_point(get_viewport().get_mouse_position()):
			return
		big_text_wnd.visible = false
		mouse_blocker.visible = false
		g.close_tooltip()
		if force_click_rect != null:
			var evt = InputEventMouseButton.new()
			evt.button_index = BUTTON_LEFT
			evt.position = force_click_rect.position + force_click_rect.size / 2
			evt.pressed = true
			get_tree().input_event(evt)
			evt.pressed = false
			get_tree().input_event(evt)
			force_click_rect = null
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
	yield(get_tree(), 'idle_frame')
	# Try to get the node immediately for efficiency
	var node = try_get_node(path)
	if node != null:
		return node
	# If not found, poll for it
	desire_path = path
	node = yield(self, 'node_found')
	desire_path = null
	return node

func show_big_window(text):
	open_time = OS.get_ticks_msec()
	big_text.bbcode_text = text
	mouse_blocker.visible = true
	mouse_blocker.material.set_shader_param('radius', 0.0);
	big_text_wnd.visible = true
	yield(self, 'popup_closed')

func show_tooltip(target, text, force=false):
	open_time = OS.get_ticks_msec()
	g.summon_tooltip(target, text)
	var rect = target.get_global_rect()
	if force:
		force_click_rect = rect
	g.tooltip.mouse_filter = MOUSE_FILTER_IGNORE
	mouse_blocker.material.set_shader_param('radius', 10.0);
	mouse_blocker.material.set_shader_param('position', rect.position + (rect.size / 2.0));
	mouse_blocker.material.set_shader_param('size', rect.size / 2.0);
	mouse_blocker.visible = true
	yield(self, 'popup_closed')

func play_button_pressed_override(select_root):
	# Check that book list contains Conjuration
	var contains_conjuration = false
	for book in select_root.chosen_books:
		if book == null:
			continue
		if book.get_id() == "conjuration":
			contains_conjuration = true
			break
	if contains_conjuration:
		select_root.on_play_button_pressed()
	else:
		show_big_window('You need to select the [i]Book of Conjuration[/i] for this tutorial.')

var picked_wrong_spells = 0
func end_turn_button_pressed_override(game_root):
	if finished_first_turn:
		game_root.on_end_turn_button_pressed()
		return

	# Confirm that the player is doing the right first turn
	var spells = game_root.my_spell_list.spells
	if spells != ['conjuration_tech', 'conjuration_attack_1']:
		picked_wrong_spells += 1
		if picked_wrong_spells <= 2:
			show_big_window('For your first turn, learn the first spell from the [b]Book of Conjuration[/b], then cast [b]Flame Strike[/b].')
		else:
			show_big_window('Press the green up arrow on the [b]Book of Conjuration[/b] (the one with the flame on it), then tap the book to open it and cast [b]Flame Strike[/b].')
		game_root.my_spell_list.spells = []
		game_root.redraw()
	else:
		game_root.on_end_turn_button_pressed()

func play_tutorial():
	# Book Select

	var selected_books_hbox = yield(self.acquire_node('ui/selected_books_hbox'), 'completed')
	var select_root = yield(self.acquire_node(''), 'completed')
	var play_button = yield(self.acquire_node('ui/play_button'), 'completed')
	# Set up Play button override
	g.safe_disconnect(play_button, 'pressed', select_root, 'on_play_button_pressed')
	play_button.connect('pressed', self, 'play_button_pressed_override', [select_root])
	# Show information
	yield(show_big_window("[b]Welcome to Adrestia![/b]\n\nYour objective is to use your spells and your wit to defeat your enemy."), 'completed')
	var books_hbox = yield(self.acquire_node('ui/books_scroll/books_hbox'), 'completed')
	yield(show_tooltip(books_hbox.get_child(1).get_child(0), 'Tap this book to see what spells it contains.', true), 'completed')

	var spell_preview = yield(self.acquire_node('ui/spell_button_list'), 'completed')
	yield(show_tooltip(spell_preview, 'Each book has four spells.'), 'completed')
	var spell_button = spell_preview.get_child(1).get_child(1)
	var mana_indicator = spell_button.get_node('cost/mp_icon')
	yield(show_tooltip(mana_indicator, "This is the spell's mana cost."), 'completed')
	yield(show_tooltip(spell_button, 'Tap the spell to see what it does.', true), 'completed')
	yield(g, 'tooltip_closed')
	var book_slot = yield(self.acquire_node('ui/selected_books_hbox'), 'completed')
	yield(show_tooltip(book_slot, 'The [b]Book of Conjuration[/b] is a good book for beginners. Drag it up here to bring it into battle.'), 'completed')
	yield(select_root, 'chose_book')
	yield(show_tooltip(play_button, "Good job! Pick two more books if you'd like, then press the play button to fight against an AI opponent."), 'completed')

	# Game
	var spell_select = yield(self.acquire_node('ui/spell_select'), 'completed')
	var game_root = yield(self.acquire_node(''), 'completed')
	var end_turn_button = yield(self.acquire_node('ui/end_turn_button'), 'completed')
	# Override end turn button
	g.safe_disconnect(end_turn_button, 'pressed', game_root, 'on_end_turn_button_pressed')
	end_turn_button.connect('pressed', self, 'end_turn_button_pressed_override', [game_root])
	# Show information
	yield(show_big_window('Nice work! Let\'s take a look around the game screen.\n\n[i]Tap to continue[/i]'), 'completed')
	var my_stats = yield(self.acquire_node('ui/my_avatar'), 'completed')
	# TODO jim: Demonstrate that spells can increase mana regen.
	yield(show_tooltip(my_stats, 'This is you! You have 25 health and 3 mana. The (+3) beside your mana shows your mana regeneration; you\'ll get this much mana at the start of each turn.'), 'completed')

	var book_button = spell_select.get_node('book_buttons').get_child(0)
	var tech_level = book_button.get_node('level')
	var upgrade_arrow = book_button.get_node('upgrade_arrow')
	yield(show_tooltip(upgrade_arrow, 'Tap this arrow to learn the next spell from the book.', true), 'completed')
	yield(show_tooltip(book_button, 'Great! Now tap the book to open it up.', true), 'completed')
	var spell_select_animator = spell_select.get_node('animation_player')
	yield(spell_select_animator, 'animation_finished')
	var buy_spell_buttons = spell_select.get_node('spell_panel/ninepatch/hbox')
	var flame_strike_button = buy_spell_buttons.get_child(0)
	yield(show_tooltip(flame_strike_button, 'Cast [b]Flame Strike[/b].', true), 'completed')
	yield(show_tooltip(end_turn_button, 'Now, end your turn.', true), 'completed')
	var spell_animation_area = game_root.get_node('ui/spell_animation_area')
	game_root.animate_events = false
	yield(show_tooltip(spell_animation_area, 'Your spells and your opponent\'s spells take effect at the same time.'), 'completed')
	game_root.animate_events = true
	yield(game_root, 'turn_animation_finished')
	finished_first_turn = true
	var mana_box = my_stats.get_node('mana_box')
	yield(show_tooltip(mana_box, 'You didn\'t use all of your mana last turn, so you have some extra now: 2 from last turn, plus 3 from this turn.'), 'completed')
	yield(show_tooltip(book_button, 'Open up the [b]Book of Conjuration[/b] again.', true), 'completed')
	yield(spell_select_animator, 'animation_finished')
	flame_strike_button = buy_spell_buttons.get_child(0)
	var fortress_button = buy_spell_buttons.get_child(1)
	yield(show_tooltip(fortress_button, '[b]Fortress[/b] is a shield spell that blocks damage. You can\'t cast it right now.'), 'completed')
	yield(show_tooltip(flame_strike_button, 'Cast [b]Flame Strike[/b] instead.', true), 'completed')
	var close_book_button = spell_select.get_node('spell_panel/ninepatch/close_button')
	yield(show_tooltip(close_book_button, 'Close the book.', true), 'completed')
	yield(show_tooltip(upgrade_arrow, 'Tap the arrow to learn the next spell from the book.', true), 'completed')
	var my_spell_list = game_root.get_node('ui/my_spell_list')
	yield(show_tooltip(my_spell_list, 'Great! Notice that you don\'t have to learn spells as your first spell, but learning does take up a spell slot. You can also only learn one spell per turn.'), 'completed')
	yield(show_tooltip(book_button, 'Open the book again.', true), 'completed')
	yield(spell_select_animator, 'animation_finished')
	fortress_button = buy_spell_buttons.get_child(1)
	yield(show_big_window('You can cast three spells per turn, which happen in order. Each spell can affect the ones that come later.'), 'completed')
	yield(show_tooltip(fortress_button, 'You just learned the next spell from the [b]Book of Conuration[/b], which was [b]Fortress[/b]. Cast it as your final spell for this turn.', true), 'completed')
	yield(show_tooltip(end_turn_button, 'End your turn.', true), 'completed')
	game_root.animate_events = false
	yield(show_tooltip(spell_animation_area, 'Spell ordering matters for your opponent too. Their first [b]Flame Strike[/b] will hit you, but their second [b]Flame Strike[/b] will get blocked by your [b]Fortress[/b].'), 'completed')
	game_root.animate_events = true
	yield(game_root, 'turn_animation_finished')
	yield(show_big_window('You\'re on your own now. Good luck!'), 'completed')

	# Results
	var results_text = yield(self.acquire_node('ui/results_text'), 'completed')
	var results_root = yield(self.acquire_node(''), 'completed')
	if results_root.winner == 0:
		yield(show_big_window('Nice job! You\'re ready to test your skills against a real opponent.'), 'completed')
	else:
		yield(show_big_window('Looks like you didn\'t win. Maybe you should try the tutorial again.'), 'completed')

	# Clean self up
	self.get_parent().remove_child(self)
