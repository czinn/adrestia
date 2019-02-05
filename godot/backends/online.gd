extends Object

# Private, do not touch
var g = null
var state = null
var started_callback = null
var update_callback = null
var in_game = false

# Public
var rules

func _init(g_):
	g = g_
	# TODO: These rules are possibly too new if we've reconnected a game that was
	# started before the rules changed. Make sure that we get the rules from the
	# server when we reconnect.
	rules = g.get_default_rules()

func get_view():
	if state == null:
		return null
	var view = g.GameView.new()
	view.init(state, 0)
	return view

func get_state():
	if state == null:
		return null
	if state.winners().size() != 0:
		return state
	else:
		return null

func register_started_callback(callback_):
	started_callback = callback_
	if in_game:
		started_callback.call_func()

func register_update_callback(callback_):
	update_callback = callback_

func submit_books(selected_book_ids):
	g.network.register_handler('push_active_games', funcref(self, 'on_push_active_games'))
	g.network.matchmake_me(g.rules, selected_book_ids, funcref(self, 'on_enter_matchmake_queue'))

func on_enter_matchmake_queue(response):
	print('We have entered the matchmaking queue:')
	print(response)

func on_push_active_games(response):
	print('Got an update:')
	print('We are now in a game: %s' % [response.game_uids])
	state = g.GameState.new()
	state.init_json(rules, response.game_states[0])
	in_game = true
	started_callback and started_callback.call_func()

func submit_action(action):
	if not state.is_valid_action(0, action):
		return false
	# TODO jim: send this shit to the server, etc
	var events = state.simulate_events([action, null]) # TODO
	if update_callback != null:
		update_callback.call_func(get_view(), events)
	return true

func leave_game():
	pass
