extends Control

signal node_found(node)
signal popup_closed()

onready var g = get_node('/root/global')

onready var mouse_blocker = $mouse_blocker
onready var big_text_wnd = $nine_patch_rect
onready var tree_poll_timer = $tree_poll_timer
onready var big_text = $nine_patch_rect/margin_container/rich_text_label

var finished_first_turn = false

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
	big_text.bbcode_text = text
	mouse_blocker.visible = true
	mouse_blocker.material.set_shader_param('radius', 0.0);
	big_text_wnd.visible = true
	yield(self, 'popup_closed')

func show_tooltip(target, text):
	g.summon_tooltip(target, text)
	var rect = target.get_global_rect()
	mouse_blocker.material.set_shader_param('radius', max(rect.size.x, rect.size.y) / 2.0 + 15.0);
	mouse_blocker.material.set_shader_param('position', rect.position + (rect.size / 2.0));
	mouse_blocker.visible = true
	yield(g, 'tooltip_closed')
	mouse_blocker.visible = false

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

func end_turn_button_pressed_override(game_root):
	if finished_first_turn:
		game_root.on_end_turn_button_pressed()
		return

	# Confirm that the player is doing the right first turn
	var spells = game_root.my_spell_list.spells
	if spells != ['conjuration_tech', 'conjuration_attack_1']:
		#show_big_window('For your first turn, level up your knowledge of the [i]Book of Conjuration[/i], and then cast [i]Ripple Shield[/i] followed by [i]Flame Strike[/i].')
		show_big_window('For your first turn, level up the [b]Book of Conjuration[/b], then cast [b]Flame Strike[/b].')
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
	#yield(show_big_window('[b]Welcome to Adrestia![/b]\n\nIn Adrestia, players cast spells in order to reduce their opponent\'s health to zero. The first player to do so wins!'), 'completed')
	yield(show_big_window("[b]Welcome to Adrestia![/b]\n\nYour objective is to use your spells and your wit to defeat your enemy."), 'completed')
	var books_hbox = yield(self.acquire_node('ui/books_scroll/books_hbox'), 'completed')
	#yield(show_tooltip(books_hbox.get_child(0), 'A book contains four spells.\n\nEach player secretly chooses three books at the beginning of the game.'), 'completed')
	yield(show_tooltip(books_hbox.get_child(1).get_child(0), 'Tap this book to see what spells it contains.'), 'completed')
	#yield(show_big_window('Choose a book by dragging it to one of the slots at the top.'), 'completed')
	while true:
		var book = yield(select_root, 'show_book_detail')
		if book.get_id() == 'conjuration':
			break

	var spell_preview = yield(self.acquire_node('ui/spell_button_list'), 'completed')
	yield(show_tooltip(spell_preview, 'Each book has four spells.'), 'completed')
	var spell_button = spell_preview.get_child(1).get_child(1)
	var mana_indicator = spell_button.get_node('cost/mp_icon')
	yield(show_tooltip(mana_indicator, "This is the spell's mana cost."), 'completed')
	yield(show_tooltip(spell_button, 'Hold down the spell to see what it does.'), 'completed')
	yield(g, 'tooltip_closed')
	#yield(show_big_window('The number in the blue icon shows how much each spell costs.\n\nYou can tap and hold a spell to see what it does.'), 'completed')
	#yield(show_big_window('The green book in the corner of each spell shows how much knowledge is required to cast the spell.\n\nYou can get up to one knowledge per turn in the book of your choice.'), 'completed')
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
	yield(show_big_window('Nice work! Let\'s take a look around the game screen.'), 'completed')
	var my_stats = yield(self.acquire_node('ui/my_avatar'), 'completed')
	# TODO jim: Demonstrate that spells can increase mana regen.
	yield(show_tooltip(my_stats, 'Here is your remaining health and mana. The (+3) beside your mana shows your mana regeneration.'), 'completed')
	yield(show_tooltip(spell_select, 'Here are your books. You can tap a book to see its spells.'), 'completed')

	var book_button = spell_select.get_node('book_buttons').get_child(0)
	var tech_level = book_button.get_node('level')
	var upgrade_arrow = book_button.get_node('upgrade_arrow')
	#yield(show_tooltip(tech_level, "This is the number of spells you've learned from this book so far."))
	#yield(show_tooltip(spell_select, 'The number on each book is how much knowledge you have of that book. You can learn the next spell from a book by tapping the green arrow.'), 'completed')
	#yield(show_big_window('For your first turn, level up your knowledge of the [i]Book of Conjuration[/i], and then cast [i]Ripple Shield[/i] followed by [i]Flame Strike[/i].'), 'completed')
	yield(show_tooltip(upgrade_arrow, 'Press this arrow to learn the next spell from the book.'), 'completed')
	while true:
		var spell = yield(spell_select, 'spell_press')
		if spell.get_id() == 'conjuration_tech':
			break
	yield(show_tooltip(book_button, 'Great! Now tap the book to open it up.'), 'completed')
	while true:
		var book = yield(spell_select, 'book_opened')[1]
		if book.get_id() == 'conjuration':
			break
	var buy_spell_buttons = spell_select.get_node('spell_panel/ninepatch/hbox')
	var flame_strike_button = buy_spell_buttons.get_child(0)
	yield(show_tooltip(flame_strike_button, 'Cast a flame strike.'), 'completed')
	while true:
		var spell = yield(spell_select, 'spell_press')
		if spell.get_id() == 'conjuration_attack_1':
			break
	yield(show_tooltip(end_turn_button, 'Now, end your turn.'), 'completed')
	yield(end_turn_button, 'pressed')
	yield(game_root, 'turn_animation_finished')
	finished_first_turn = true
	#yield(show_big_window('Nice job! So what happened? Your opponent cast two Flame Strikes, both of which were blocked by your shield, and your Flame Strike dealt three damage!'), 'completed')
	yield(show_big_window('Nice job! You\'re on your own now. Good luck!'), 'completed')

	# Results
	var results_text = yield(self.acquire_node('ui/results_text'), 'completed')
	var results_root = yield(self.acquire_node(''), 'completed')
	if results_root.winner == 0:
		yield(show_big_window('Nice job! You\'re ready to test your skills against a real opponent.'), 'completed')
	else:
		yield(show_big_window('Looks like you didn\'t win. Maybe you should try the tutorial again.'), 'completed')

	# Clean self up
	self.get_parent().remove_child(self)
