extends Node

signal turn_animation_finished

onready var g = get_node('/root/global')

onready var spell_button_list_scene = preload('res://components/spell_button_list.tscn')

onready var game = $ui
onready var spell_select = $ui/spell_select
onready var end_turn_button = $ui/end_turn_button
onready var my_spell_list = $ui/my_spell_list
onready var my_avatar = $ui/my_avatar
onready var my_stickies = $ui/my_stickies
onready var enemy_avatar = $ui/enemy_avatar
onready var enemy_spell_list = $ui/enemy_spell_list
onready var enemy_stickies = $ui/enemy_stickies
onready var event_timer = $ui/event_timer
onready var animation_player = $ui/animation_player
onready var back_button = $ui/back_button

var state
var events = []

enum UiState {
	CHOOSING_SPELLS,
	WAITING_FOR_UPDATE,
	SHOWING_SIMULATION
}
var ui_state

func _ready():
	state = g.GameState.new()
	state.of_game_view(g.backend.get_view())
	ui_state = UiState.CHOOSING_SPELLS
	g.backend.register_update_callback(funcref(self, 'on_backend_update'))
	end_turn_button.connect('pressed', self, 'on_end_turn_button_pressed')
	my_spell_list.connect('pressed', self, 'on_my_spell_list_pressed')
	event_timer.connect('timeout', self, 'on_event_timer_timeout')
	my_spell_list.spells = []
	enemy_spell_list.immediately_show_tooltip = true
	spell_select.display_filter = funcref(self, 'is_not_tech_spell')
	spell_select.enabled_filter = funcref(self, 'player_can_cast')
	spell_select.unlocked_filter = funcref(self, 'player_has_unlocked_spell')
	spell_select.books = state.players[0].books
	spell_select.connect('spell_press', self, 'on_spell_enqueue')
	back_button.connect('pressed', self, 'on_back_button_pressed')
	get_tree().set_auto_accept_quit(false)
	yield(get_tree(), 'idle_frame')
	redraw()

func _notification(what):
	print(what)
	if what == MainLoop.NOTIFICATION_WM_QUIT_REQUEST:
		self.call_deferred('on_back_button_pressed')

func on_back_button_pressed():
	var confirmed = yield(g.summon_confirm('[center]Are you sure you want to forfeit?[/center]'), 'popup_closed')
	if confirmed:
		g.backend.leave_game()
		g.backend = null
		# TODO: charles: Possibly go to game-end screen instead.
		g.scene_loader.goto_scene('title', true)

func on_spell_enqueue(spell):
	var action = my_spell_list.spells.duplicate()
	action.append(spell.get_id())
	if not state.is_valid_action(0, action):
		return
	my_spell_list.spells = action
	redraw()

func on_book_upgrade(index, book):
	print(index)
	print(book)

func on_my_spell_list_pressed(index, spell):
	if ui_state == CHOOSING_SPELLS:
		var action = my_spell_list.spells.duplicate()
		action.remove(index)
		if not state.is_valid_action(0, action):
			return
		my_spell_list.spells = action
		redraw()

# TODO: jim: All the player_* functions duplicate some part of our game logic.
# Move these to helper functions in C++ and wrap them?
func player_upgraded_book_id():
	for spell_id in my_spell_list.spells:
		var spell = g.backend.rules.get_spell(spell_id)
		if spell.is_tech_spell():
			return spell.get_book()
	return null

# TODO: jim: This is O(n) in the number of spells in the queue. Not super
# important to improve, but feels bad.
func player_mp_left():
	var me = state.players[0]
	var mp_left = me.mp
	for spell_id in my_spell_list.spells:
		var spell = g.backend.rules.get_spell(spell_id)
		mp_left -= spell.get_cost()
	return mp_left

func player_can_afford(spell):
	if spell.is_tech_spell() and player_upgraded_book_id() != null:
		return false
	var me = state.players[0]
	var mp_left = player_mp_left()
	return spell.get_cost() <= mp_left

# TODO: jim: This is ugly because we have to zip through Player.tech and
# Player.books. Make it not ugly.
func player_effective_tech_in(book_id):
	var me = state.players[0]
	for i in range(len(me.tech)):
		if me.books[i].get_id() == book_id:
			return me.tech[i] + (1 if player_upgraded_book_id() == book_id else 0)
	return 0

func player_effective_tech():
	var me = state.players[0]
	var result = me.tech
	var upgraded_book_id = player_upgraded_book_id()
	for i in range(len(result)):
		if me.books[i].get_id() == upgraded_book_id:
			result[i] += 1
	return result

func player_effective_level():
	var me = state.players[0]
	return g.sum(me.tech) + (1 if player_upgraded_book_id() != null else 0)

func player_has_unlocked_spell(spell):
	return (player_effective_level() >= spell.get_level() and
			player_effective_tech_in(spell.get_book()) >= spell.get_tech())

func player_can_cast(spell):
	if spell.is_tech_spell() and player_upgraded_book_id() != null:
		return false
	return player_has_unlocked_spell(spell) && player_can_afford(spell)

func is_not_tech_spell(spell):
	return not spell.is_tech_spell()

func redraw():
	var me = state.players[0]
	var them = state.players[1]
	var mp_left = player_mp_left()
	enemy_avatar.redraw(them)
	enemy_stickies.redraw(them.stickies)
	my_avatar.redraw(me, mp_left)
	my_stickies.redraw(me.stickies)
	spell_select.tech_levels = player_effective_tech()
	my_spell_list.redraw()
	spell_select.redraw_spells()
	spell_select.redraw_tech_upgrades(player_upgraded_book_id())

func on_end_turn_button_pressed():
	spell_select.on_close_book()
	var action = my_spell_list.spells

	ui_state = UiState.WAITING_FOR_UPDATE

	animation_player.play('end_turn')
	yield(animation_player, 'animation_finished')

	my_spell_list.immediately_show_tooltip = true
	redraw()

	if not g.backend.submit_action(action):
		# TODO: charles: Show error message; this should never happen
		return

func on_backend_update(new_view, update_events):
	# Initialize the spell lists
	var last_turn = new_view.history[-1]
	my_spell_list.spells = last_turn[new_view.view_player_id]
	enemy_spell_list.spells = last_turn[1 - new_view.view_player_id]

	events = update_events
	ui_state = UiState.SHOWING_SIMULATION

func events_compatible(first_event, second_event):
	if second_event['type'] == 'time_point':
		return true
	if first_event['type'] == 'time_point' and (first_event['point'] != 'spells_fired' && first_event['point'] != 'spells_hit_or_countered'):
		return false
	if (second_event['type'] == 'sticky_amount_changed' || \
				 second_event['type'] == 'sticky_duration_changed' || \
				 second_event['type'] == 'sticky_expired') && \
			( \
				 (first_event['type'] == 'effect' && \
						first_event['effect']['kind'] == 'sticky' && \
						first_event['effect']['target_player'] == second_event['player']) || \
				 (first_event['type'] == 'sticky_amount_changed' && \
					 	first_event['player'] == second_event['player'] && \
						first_event['sticky_index'] == second_event['sticky_index']) \
			):
		return false
	if first_event['type'] == 'effect' && second_event['type'] == 'effect' && \
			first_event['effect']['kind'] == second_event['effect']['kind'] && \
			first_event['effect']['target_player'] == second_event['effect']['target_player']:
		return false
	if first_event['type'] == 'fire_spell' && second_event['type'] == 'effect' && \
			second_event['effect']['kind'] == 'mana' && \
			first_event['player'] == second_event['effect']['target_player']:
		return false
	return true

func on_event_timer_timeout():
	if events.size() == 0:
		if ui_state == UiState.SHOWING_SIMULATION:
			ui_state = UiState.CHOOSING_SPELLS

			state.of_game_view(g.backend.get_view())

			# Clear the spell lists
			my_spell_list.spells = []
			my_spell_list.immediately_show_tooltip = false
			enemy_spell_list.spells = []

			redraw()

			if len(state.winners()) > 0:
				print('Game is over')
				g.scene_loader.goto_scene('game_results')
			else:
				animation_player.play_backwards('end_turn')
				emit_signal('turn_animation_finished')

		return

	var events_to_show = []

	while events.size() > 0:
		var is_compatible = true
		for prior_event in events_to_show:
			if not events_compatible(prior_event, events[0]):
				is_compatible = false
				break
		if is_compatible:
			var event = events.pop_front()
			if event['type'] == 'time_point' and events_to_show.size() == 0:
				continue
			events_to_show.append(event)
		else:
			break

	for event in events_to_show:
		state.apply_event(event)
	
		var me = state.players[0]
		var them = state.players[1]
		# Do UI effects for event
		if event['type'] == 'fire_spell':
			var player_spell_list = my_spell_list if event['player'] == 0 else enemy_spell_list
			player_spell_list.flash_spell(event['index'])
			# We need to update mana here because spells cost mana
			if event['player'] == 0:
				my_avatar.redraw(me)
			else:
				enemy_avatar.redraw(them)
		elif event['type'] == 'player_mp':
			if event['player'] == 0:
				my_avatar.redraw(me)
			else:
				enemy_avatar.redraw(them)
		elif event['type'] == 'effect':
			var effect = event['effect']
			var target_player = effect['target_player']
			var kind = effect['kind']
			if kind == 'health' or kind =='mana_regen' or kind == 'mana':
				if target_player == 0:
					my_avatar.redraw(me)
				else:
					enemy_avatar.redraw(them)
			elif kind == 'tech':
				spell_select.tech_levels = player_effective_tech()
			elif kind == 'sticky':
				if target_player == 0:
					my_stickies.redraw_append(me.stickies)
				else:
					enemy_stickies.redraw_append(them.stickies)
		elif event['type'] == 'spell_countered':
			var player_spell_list = my_spell_list if event['player'] == 0 else enemy_spell_list
			player_spell_list.spell_countered(event['index'])
		elif event['type'] == 'spell_hit':
			pass # TODO: charles: Maybe animate this
		elif event['type'] == 'sticky_amount_changed' or event['type'] == 'sticky_duration_changed':
			if event['player'] == 0:
				my_stickies.redraw_update(me.stickies)
			else:	
				enemy_stickies.redraw_update(them.stickies)
		elif event['type'] == 'sticky_expired':
			if event['player'] == 0:
				my_stickies.redraw_remove(event['sticky_index'])
			else:
				enemy_stickies.redraw_remove(event['sticky_index'])
