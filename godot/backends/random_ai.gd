extends Object

signal debug_timer

# Private, do not touch
var g = null
var state = null
var ai = null
var started_callback = null
var update_callback = null
var left_game = false

var do_debug_timer = true

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
	if left_game: return null
	if state == null:
		return null
	var view = g.GameView.new()
	view.init(state, 0)
	return view

func get_state():
	if left_game: return null
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

func emit_debug_timer():
	emit_signal('debug_timer')

func submit_books(selected_book_ids):
	if do_debug_timer:
		var timer = Timer.new()
		timer.set_one_shot(true)
		timer.set_timer_process_mode(0)
		timer.set_wait_time(2.0)
		timer.connect('timeout', self, 'emit_debug_timer')
		g.get_node('/root').add_child(timer)
		timer.start()
		yield(self, 'debug_timer')

	if left_game: return
	state = g.GameState.new()
	state.init(g.rules, [selected_book_ids, selected_book_ids])
	if started_callback != null:
		started_callback.call_func()

func submit_action(action):
	if left_game: return false
	if not state.is_valid_action(0, action):
		return false

	var view = g.GameView.new()
	view.init(state, 1)
	var ai_action = ai.get_action(view)

	var events = state.simulate_events([action, ai_action])

	if update_callback != null:
		update_callback.call_func(get_view(), events)

	return true

func leave_game():
	left_game = true
