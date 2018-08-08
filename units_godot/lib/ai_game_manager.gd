extends 'game_manager.gd'

const GameState = preload('res://native/game_state.gdns')
const Tech = preload('res://native/tech.gdns')

var game_state
var units

func _ready():
  game_state = GameState.new()

func perform_action(action):
  action.player = 0
  gs.perform_action(action)

func start_game(callback_obj, callback):
  # AI resource selection
  randomize()
  var x = randi() % 4
  var y = randi() % 4
  var r = 1 + min(x, y)
  var g = 1 + abs(x - y)
  var b = 7 - r - g
  gs.perform_action(ChooseResources.new(1, Resources.new(r, g, b)))
  # Start the game
  gs.start_game()
  callback_obj.call(callback)

func get_view():
  return gs

func end_turn(callback_obj, callback):
  # AI move
  randomize()
  var resources = get_view().players[1].resources
  for i in range(100):
    if resources.total() <= 1:
      break
    var u = randi() % units.size()
    var kind = units[units.keys()[u]]
    if kind.cost != null && resources.subsumes(kind.cost):
      gs.perform_action(BuildUnit.new(1, kind))
  callback_obj.call(callback)

func simulate_battle(callback_obj, callback):
  # Actually run battle
  var result = gs.simulate_battle()
  callback_obj.call(callback, result)
