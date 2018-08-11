extends SceneTree

const GameRules = preload('res://native/game_rules.gdns')
const GameState = preload('res://native/game_state.gdns')
const Colour = preload('res://native/colour.gdns')
const Action = preload('res://native/action.gdns')
const Tech = preload('res://native/tech.gdns')
const UnitKind = preload('res://native/unit_kind.gdns')

func trial():
  print('BEGIN')
  var file = File.new()
  file.open('res://data/rules.json', file.READ)
  var rules = GameRules.new()
  rules.load_json_string(file.get_as_text())
  print('END')
  #var tech = Tech.new()

  #print(rules)
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

func _init():
  while true:
    trial()
  quit()
