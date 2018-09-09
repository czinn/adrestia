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
  # TODO: jim: Return and use a PlayerView instead. We currently use GameState
  # because we don't have a holistic structure for a player's information in
  # the game (GameView doesn't contain the current player's tech).
  return game_state

func end_turn(callback_obj, callback):
  # Enemy AI picks a random tech, then buys random units.
  # TODO: jim: Hook into C++ AI instead of writing AI code in gdscript.

  var action = Action.new()
  action.init_tech_colour([g.Colour_.RED, g.Colour_.GREEN, g.Colour_.BLUE][randi()%3])
  game_state.perform_action(1, action)
  var affordable_kinds = []
  var me = game_state.get_players()[1]
  for kind_id in g.unit_kinds:
    var kind = g.unit_kinds[kind_id]
    if kind.get_tech().not_null() and me.tech.includes(kind.get_tech()):
      affordable_kinds.append(kind)
  var total_cost = 0
  var units_to_buy = []
  for i_ in range(100):
    var kind = affordable_kinds[randi() % affordable_kinds.size()]
    if total_cost + kind.get_cost() <= me.coins:
      units_to_buy.append(kind.get_id())
      total_cost += kind.get_cost()
  action.init_units(units_to_buy)
  game_state.perform_action(1, action)

  # This last action triggers a battle.
  var battle = game_state.get_battles().back()

  callback_obj.call(callback, battle)
