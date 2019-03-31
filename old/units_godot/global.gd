extends Node

const AiGameManager = preload('res://lib/ai_game_manager.gd')
const Action = preload('res://native/action.gdns')
const Colour = preload('res://native/colour.gdns')
const GameRules = preload('res://native/game_rules.gdns')
const GameState = preload('res://native/game_state.gdns')

var rules
var unit_kinds
var man

# So our naming convention here is apparently:
# Thing: Thin wrapper over C++ pointer.;
# Thing_: Object that provides gdscript-level conveniences.
# Thing__: Class for Thing_.
class Colour__:
  var RED = Colour.new()
  var GREEN = Colour.new()
  var BLUE = Colour.new()

  func _init():
    RED.set_RED()
    GREEN.set_GREEN()
    BLUE.set_BLUE()

  func of_char(c):
    match c:
      'r': return RED
      'g': return GREEN
      'b': return BLUE

var Colour_

func _init():
  Colour_ = Colour__.new()

# Global values for testing individual scenes.
# Should be overridden when playing the actual game.
func _ready():
  var file = File.new()
  file.open('res://data/rules.json', file.READ)
  rules = GameRules.new()
  rules.load_json_string(file.get_as_text())
  unit_kinds = rules.get_unit_kinds()
  # jim: We initialize man to a good state here so that we can use Godot's play
  # scene feature.
  man = AiGameManager.new(rules, self)

func new_ai_game():
  man = AiGameManager.new(rules, self)
  get_tree().change_scene("res://scenes/game.tscn")
