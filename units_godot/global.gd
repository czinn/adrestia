extends Node

const GameRules = preload('res://native/game_rules.gdns')
const GameState = preload('res://native/game_state.gdns')
const AiGameManager = preload('res://lib/ai_game_manager.gd')
var rules
var man

# Global values for testing individual scenes.
# Should be overridden when playing the actual game.
func _ready():
  var file = File.new()
  file.open('res://data/rules.json', file.READ)
  rules = GameRules.new()
  rules.load_json_string(file.get_as_text())
  # jim: We initialize man to a good state here so that we can use Godot's play
  # scene feature.
  man = AiGameManager.new(rules)

func new_ai_game():
  man = AiGameManager.new(rules)
  get_tree().change_scene("res://scenes/game.tscn")
