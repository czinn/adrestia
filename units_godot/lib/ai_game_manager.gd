extends Node

# TODO: jim: onready var g = get_node('/root/global') does NOT work; it results
# in a Null g. I have no idea why.
var g
const Action = preload('res://native/action.gdns')
const GameState = preload('res://native/game_state.gdns')
const Tech = preload('res://native/tech.gdns')

var game_state = GameState.new()

func _init(rules_, g_):
  g = g_
  game_state.init(rules_, 2)

func perform_action(action):
  game_state.perform_action(0, action)

func start_game(callback_obj, callback):
  callback_obj.call(callback)

func get_view():
  # TODO(jim): Return and use a PlayerView instead. We currently use GameState
  # because we don't have a holistic structure for a player's information in
  # the game (GameView doesn't contain the current player's tech).
  return game_state

func end_turn(callback_obj, callback):
  # TODO(jim): Random AI move

  var action = Action.new()
  action.init_tech_colour(g.Colour_.RED)
  game_state.perform_action(1, action)
  action.init_units(['grunt'])
  game_state.perform_action(1, action)

  # This last action triggers a battle.
  var battle = game_state.get_battles().back()

  # jim: This is Charles's stuff.
  #var resources = get_view().players[1].resources
  #for i in range(100):
  #  if resources.total() <= 1:
  #    break
  #  var u = randi() % units.size()
  #  var kind = units[units.keys()[u]]
  #  if kind.cost != null && resources.subsumes(kind.cost):
  #    gs.perform_action(BuildUnit.new(1, kind))
  callback_obj.call(callback, battle)
