extends Object

# Private, do not touch
var g = null
var state = null
var started_callback = null
var update_callback = null

# Public
var rules

func _init(g_):
	g = g_
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

func register_update_callback(callback_):
	update_callback = callback_

func submit_books(selected_book_ids):
	state = g.GameState.new()
	state.init(g.rules, [selected_book_ids, ['conjuration']])
	if started_callback != null:
		started_callback.call_func()

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
