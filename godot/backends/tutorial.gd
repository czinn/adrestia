extends Object

# Private, do not touch
var g = null
var state = null
var started_callback = null
var update_callback = null

var turn = 0
var scripted_actions = [
	['bloodlust_tech', 'bloodlust_1'],
	['bloodlust_tech', 'bloodlust_1', 'bloodlust_1'],
	['bloodlust_1', 'bloodlust_1'],
	['bloodlust_1'],
]

# Public
var rules
var forfeited = false

func _init(g_):
	g = g_
	# Always load these from the local file, that way we don't have to worry about
	# breaking the tutorial when we update the rules.
	var rules_file = File.new()
	rules_file.open('res://data/rules.json', File.READ)
	rules = g.GameRules.new()
	rules.load_json_string(rules_file.get_as_text())
	rules_file.close()

func get_time_limit():
	return 0

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

func get_current_move():
	return null

func get_opponent():
	return null

func register_started_callback(callback_):
	started_callback = callback_

func register_update_callback(callback_):
	update_callback = callback_

func submit_books(selected_book_ids):
	state = g.GameState.new()
	state.init(g.get_rules(), [selected_book_ids, ['bloodlust']])
	if started_callback != null:
		started_callback.call_func()

func submit_action(action):
	if not state.is_valid_action(0, action):
		return false

	var ai_action = scripted_actions.back()
	if turn < len(scripted_actions):
		ai_action = scripted_actions[turn]
	turn += 1

	var events = state.simulate_events([action, ai_action])

	if update_callback != null:
		update_callback.call_func(get_view(), events)

	return true

func leave_game():
	pass
