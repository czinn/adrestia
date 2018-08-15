extends SceneTree

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
  var unit_kinds = rules.get_unit_kinds()
  for unit_name in unit_kinds:
    var unit_kind = unit_kinds[unit_name]
    var tech = unit_kind.get_tech()
    if tech.not_null():
      print(tech.as_json().result)
  print('END')
  #var tech = Tech.new()

  #print(rules.as_json().result)
  #print(rules.as_json().result.unit_kinds[0].tech)
  #print(rules.as_json().result.unit_kinds[0].tech)

  #tech.load_json_string(JSON.print(rules.as_json().result.unit_kinds[0].tech))
  #JSON.print(rules.as_json().result.unit_kinds[0].tech)
  ##print(tech)
  ##print(tech.includes(tech))

  #print(rules.get_unit_cap())

  #var one_kind = rules.get_unit_kind('avatar')
  #print(one_kind)
  #print(one_kind.get_id())

  #var unit_kinds = rules.get_unit_kinds()
  #print(unit_kinds)
  #print(unit_kinds['avatar'])
  #print(unit_kinds['avatar'].get_id())

  #var state = GameState.new();
  #state.init(rules, 2)

  #var colour = Colour.new();
  #colour.load_json_string('"RED"')
  #var action = Action.new();
  #action.init_tech_colour(colour)

  #state.perform_action(0, action)

  #colour.load_json_string('"GREEN"')
  #action.init_tech_colour(colour)
  #state.perform_action(1, action)

  #action.init_units(['grunt', 'grunt'])
  #state.perform_action(0, action)
  #state.perform_action(1, action)

  #print(state.as_json().result)

#37744
func _init():
  print('Begin of all')
  rules_file = File.new()
  rules_file.open('res://data/rules.json', File.READ)
  rules_text = rules_file.get_as_text()
  rules.load_json_string(rules_text)
  rules_file.close()
  for i in range(5):
    trial()
  quit()
