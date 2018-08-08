extends SceneTree

const GameRules = preload('res://native/game_rules.gdns')
const GameState = preload('res://native/game_state.gdns')

func _init():
  var file = File.new()
  file.open('res://data/rules.json', file.READ)
  var rules = GameRules.new()
  rules.load_json_string(file.get_as_text())
  print(rules.as_json().result)
  var state = GameState.new()
  state.init(rules, 2)
  print(state.as_json().result)
