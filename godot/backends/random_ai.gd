extends Object

# Private, do not touch
var g = null
var state = null
var ai = null
var callback = null

# Public
var rules

func _init(g_):
	g = g_
	var rules_file = File.new()
	rules_file.open('res://data/rules.json', File.READ)
	rules = g.GameRules.new()
	rules.load_json_string(rules_file.get_as_text())
	rules_file.close()
	ai = g.Strategy.new()
	ai.init_random_strategy()

func get_view():
	var view = g.GameView.new()
	view.init(state, 0)
	return view

func get_state():
	if state.winners().size() != 0:
		return state
	else:
		return null

func register_update_callback(callback_):
	callback = callback_

func submit_books(selected_book_ids):
	state = g.GameState.new()
	state.init(g.rules, [selected_book_ids, selected_book_ids])

func submit_action(action):
	if not state.is_valid_action(0, action):
		return false

	var view = g.GameView.new()
	view.init(state, 1)
	var ai_action = ai.get_action(view)

	var events = state.simulate_events([action, ai_action])

	if callback != null:
		callback.call_func(get_view(), events)

	return true
