extends Node

const GameRules = preload('res://native/game_rules.gdns')
const GameState = preload('res://native/game_state.gdns')
const AiGameManager = preload('res://lib/ai_game_manager.gd')
var rules
var man

# jim: We initialize man to a good value here so that we can
# use the Play Scene feature in Godot.
onready var man = AiGameManager.new(units) # Game manager

# Global values for testing individual scenes.
# Should be overridden when playing the actual game.
func _ready():
  var file = File.new()
  file.open('res://data/rules.json', file.READ)
  rules = GameRules.new()
  rules.load_json_string(file.get_as_text())

func new_ai_game():
  man = AiGameManager.new(units)
  get_tree().change_scene("res://scenes/game.tscn")
