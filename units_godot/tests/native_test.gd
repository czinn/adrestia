extends SceneTree

const GameRules = preload('res://native/game_rules.gdns')
const GameState = preload('res://native/game_state.gdns')
const Colour = preload('res://native/colour.gdns')
const Action = preload('res://native/action.gdns')

func _init():
  var file = File.new()
  file.open('res://data/rules.json', file.READ)

  var rules = GameRules.new()
  rules.load_json_string(file.get_as_text())
  print(rules.as_json().result)

  var state = GameState.new();
  state.init(rules, 2)
  print(state.as_json().result)

  var colour = Colour.new();
  colour.load_json_string('"RED"')
  var action = Action.new();
  action.init_tech_colour(colour)

  state.perform_action(0, action)
  print(state.as_json().result)

  quit()
