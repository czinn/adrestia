extends Object

# Private, do not touch
var g = null
var state = null
var callback = null

var turn = 0
var scripted_actions = [
	['conjuration_tech', 'conjuration_attack_1'],
	['conjuration_tech', 'conjuration_attack_1'],
	['conjuration_attack_1'],
]

# Public
var rules

func _init(g_):
	g = g_
	var rules_file = File.new()
	rules_file.open('res://data/rules.json', File.READ)
	rules = g.GameRules.new()
	rules.load_json_string(rules_file.get_as_text())
	rules_file.close()

func get_view():
	var view = g.GameView.new()
	view.init(state, 0)
	return view

func get_state():
	if state.get_winners().size() != 0:
		return state
	else:
		return null

func register_update_callback(callback_):
	callback = callback_

func submit_books(selected_book_ids):
	state = g.GameState.new()
	state.init(g.rules, [selected_book_ids, ['conjuration']])

func submit_action(action):
	if not state.is_valid_action(0, action):
		return false

	var ai_action = scripted_actions.back()
	if turn < len(scripted_actions):
		ai_action = scripted_actions[turn]
	turn += 1

	var events = state.simulate_events([action, ai_action])

	if callback != null:
		callback.call_func(get_view(), events)

	return true

func forfeit_game():
	pass
