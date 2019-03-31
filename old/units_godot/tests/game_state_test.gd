extends SceneTree

const UnitKinds = preload("res://lib/unit_kinds.gd")
const GameState = preload("res://lib/game_state.gd")
const ChooseResources = preload("res://lib/actions/choose_resources.gd")
const BuildUnit = preload("res://lib/actions/build_unit.gd")
const Resources = preload("res://lib/resources.gd")

func _init():
  var unit_kinds = UnitKinds.load_units()
  var state = GameState.new(unit_kinds, 2)
  assert(state.perform_action(ChooseResources.new(0, Resources.new(4, 2, 1))))
  assert(!state.perform_action(BuildUnit.new(1, unit_kinds['turret'])))
  assert(state.perform_action(ChooseResources.new(1, Resources.new(0, 7, 0))))
  assert(state.perform_action(BuildUnit.new(1, unit_kinds['turret'])))
  assert(state.perform_action(BuildUnit.new(1, unit_kinds['turret'])))
  assert(state.perform_action(BuildUnit.new(1, unit_kinds['turret'])))
  assert(!state.perform_action(BuildUnit.new(1, unit_kinds['turret'])))
  var result = state.simulate_battle()
  assert(result.attacks.size() == 5)
  assert(state.perform_action(BuildUnit.new(1, unit_kinds['turret'])))
  quit()
