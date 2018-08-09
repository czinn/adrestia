extends 'game_manager.gd'

const GameState = preload('res://native/game_state.gdns')
const Tech = preload('res://native/tech.gdns')

var game_state
var rules

func _init(rules_):
  rules = rules_

func _ready():
  game_state = GameState.new()

func perform_action(action):
  game_state.perform_action(action)

func start_game(callback_obj, callback):
  callback_obj.call(callback)

func get_view():
  # TODO(jim): Return and use a PlayerView instead.
  return game_state

func end_turn(callback_obj, callback):
  # TODO(jim): AI move

  #var resources = get_view().players[1].resources
  #for i in range(100):
  #  if resources.total() <= 1:
  #    break
  #  var u = randi() % units.size()
  #  var kind = units[units.keys()[u]]
  #  if kind.cost != null && resources.subsumes(kind.cost):
  #    gs.perform_action(BuildUnit.new(1, kind))
  callback_obj.call(callback)

func simulate_battle(callback_obj, callback):
  # TODO(jim): This is now done automatically when all players have made moves.
  # Re-architect appropriately.
  callback_obj.call(callback)
