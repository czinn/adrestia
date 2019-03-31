extends SceneTree

# Tests the integration of gdnative modules.

# Also serves as an example of how exactly to use them. Notably,
# "GameRules.new()" should not be considered an instance of GameRules, but a
# reference-counted POINTER to an instance of GameRules where most operations
# implicitly dereference the pointer. This leads to a bit of verbosity around
# creating and deleting the underlying object, as well as checking whether it
# actually exists (with .is_null() or .not_null()).

const GameRules = preload('res://native/game_rules.gdns')
const GameState = preload('res://native/game_state.gdns')
const Colour = preload('res://native/colour.gdns')
const Action = preload('res://native/action.gdns')
const Tech = preload('res://native/tech.gdns')
const UnitKind = preload('res://native/unit_kind.gdns')

var rules_file
var rules_text
var rules = GameRules.new()
var game_state = GameState.new()

func trial():
  print('BEGIN')
  game_state.init(rules, 2)
  print('Welcome to Adrestia!')
  print('Unit cap is %d' % [rules.get_unit_cap()])
  print('Units are: %s' % PoolStringArray(rules.get_unit_kinds().keys()).join(','))

  print('P1 Grunt Rush v. P2 Turret Rush.')

  # TODO(jim): Better way to create instances of enums.
  var red = Colour.new(); red.load_json_string('"RED"')
  var green = Colour.new(); green.load_json_string('"GREEN"')

  var action = Action.new()
  action.init_tech_colour(red)
  game_state.perform_action(0, action)
  action.init_tech_colour(green)
  game_state.perform_action(1, action)
  action.init_units(['grunt', 'grunt'])
  game_state.perform_action(0, action)
  action.init_units(['turret', 'turret'])
  game_state.perform_action(1, action)

  var last_battle = game_state.get_battles()[-1]
  var last_player_views = last_battle.get_players()

  for attack in last_battle.get_attacks():
    print("Player %d's %s hits Player %d's %s for %d" % [
      attack.from_player,
      last_player_views[attack.from_player].units[attack.from_unit].kind.get_name(),
      attack.to_player,
      last_player_views[attack.to_player].units[attack.to_unit].kind.get_name(),
      attack.damage])

  print('END')

func _init():
  print('Begin of all')
  rules_file = File.new()
  rules_file.open('res://data/rules.json', File.READ)
  rules_text = rules_file.get_as_text()
  rules.load_json_string(rules_text)
  rules_file.close()
  for i in range(100):
    trial()
  quit()
