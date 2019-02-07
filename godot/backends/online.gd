extends Object

# Private, do not touch
var g = null
var player_id = null
var game_uid = null
var view = null
var state = null
var started_callback = null
var update_callback = null
var in_game = false

# Public
var rules
var forfeited = false

func _init(g_):
	g = g_
	# TODO: These rules are possibly too new if we've reconnected a game that was
	# started before the rules changed. Make sure that we get the rules from the
	# server when we reconnect.
	rules = g.get_default_rules()

func get_view():
	if view != null:
		return view
	return null

func get_state():
	if state == null:
		return null
	return state

func register_started_callback(callback_):
	started_callback = callback_
	if in_game:
		started_callback and started_callback.call_func()

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
	print(response)
	var game = response.updates[0]
	game_uid = game.game_uid
	if game.has('game_state'):
		# This means the game is over.
		state = g.GameState.new()
		state.init_json(rules, game.game_state)
		if player_id != null:
			view.init(state, player_id)
		forfeited = true
	else:
		view = g.GameView.new()
		view.init_json(rules, game.game_view)
		player_id = view.view_player_id
		print('successfully created view')
	if game.events.size() > 0 or forfeited:
		update_callback.call_func(get_view(), game.events)

	if not in_game:
		print('We are now in a game: %s' % [game_uid])
		in_game = true
		started_callback and started_callback.call_func()

func on_submit_move(response):
	print('Submitted move with response: %d' % [response.api_code])

func submit_action(action):
	g.network.submit_move(game_uid, action, funcref(self, 'on_submit_move'))
	return true

func leave_game():
	g.network.abort_game(game_uid, funcref(g.network, 'print_response'))
