extends Node

const GameState = preload("res://lib/game_state.gd")
const Resources = preload("res://lib/resources.gd")
const AiGameManager = preload("res://lib/ai_game_manager.gd")
onready var units = get_node("/root/UnitKinds").units

# jim: We initialize man to a good value here so that we can
# use the Play Scene feature in Godot.
onready var man = AiGameManager.new(units) # Game manager

# Global values for testing individual scenes.
# Should be overridden when playing the actual game.
func _ready():
  man.gs.players[0].resource_gain = Resources.new(20, 20, 20)

func new_ai_game():
  man = AiGameManager.new(units)
  get_tree().change_scene("res://scenes/game.tscn")
