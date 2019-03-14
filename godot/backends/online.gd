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
var current_move = null
var opponent = null

# Public
var rules
var forfeited = false

func _notification(what):
	if what == NOTIFICATION_PREDELETE:
		g.network.disconnect('disconnected', self, 'disconnected')

func _init(g_):
	g = g_
	rules = g.get_default_rules()
	g.network.connect('disconnected', self, 'disconnected')

func disconnected():
	g.scene_loader.goto_scene('title')

func reconnect(update_message):
	var game = update_message.updates[0]
	game['events'] = []
	if 'game_rules' in game:
		rules.load_json_string(JSON.print(game['game_rules']))
	if 'player_move' in game:
		current_move = game['player_move']
	on_push_active_games(update_message)
	g.network.register_handler('push_active_games', funcref(self, 'on_push_active_games'))

func get_time_limit():
	return 60

func get_view():
	if view != null:
		return view
	return null

func get_state():
	if state == null:
		return null
	return state

func get_current_move():
	return current_move

func get_opponent():
	return opponent

func register_started_callback(callback_):
	started_callback = callback_
	if in_game:
		started_callback and started_callback.call_func()

func register_update_callback(callback_):
	update_callback = callback_

func submit_books(selected_book_ids):
	g.network.register_handler('push_active_games', funcref(self, 'on_push_active_games'))
	g.network.matchmake_me(g.get_rules(), selected_book_ids, funcref(self, 'on_enter_matchmake_queue'))

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
		if state.winners().size() == 0:
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
		# Request opponent's profile from the server
		g.network.get_user_profile(game.opponent_friend_code, funcref(self, 'on_opponent_profile'))

func on_submit_move(response):
	print('Submitted move with response: %d' % [response.api_code])

func on_opponent_profile(response):
	print(response)
	if response.api_code == 200:
		opponent = response.profile

func submit_action(action):
	if action == null:
		return
	current_move = action
	g.network.submit_move(game_uid, action, funcref(self, 'on_submit_move'))
	return true

func leave_game():
	g.network.register_handler('push_active_games', funcref(g.network, 'discard'))
	g.network.abort_game(game_uid, funcref(g.network, 'print_response'))
