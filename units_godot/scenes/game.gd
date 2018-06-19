extends VBoxContainer

var unit_buy_bar = preload('res://components/unit_buy_bar.tscn')
var BuildUnit = preload('res://lib/actions/build_unit.gd')

onready var r_label = $Toolbar/R
onready var g_label = $Toolbar/G
onready var b_label = $Toolbar/B
onready var EndTurnButton = $Toolbar/EndTurnButton
onready var UnitList = $ScrollContainer/UnitList
onready var Armies = $MarginContainer/Armies
onready var g = get_node("/root/global")
onready var units = get_node("/root/UnitKinds").units
var unit_bars = {}

func update_ui():
  var view = g.man.get_view()

  r_label.text = str(view.players[0].resources.r)
  g_label.text = str(view.players[0].resources.g)
  b_label.text = str(view.players[0].resources.b)
  for unit in unit_bars:
    unit_bars[unit].buy_button.disabled = !view.players[0].resources.subsumes(units[unit].cost)
  
  # Unit packing
  # Right now is very bad; just makes each unit very tall and puts them in a single row
  for pid in range(view.players.size()):
    var player = view.players[pid]
    var packing_data = []
    var x = 0
    for unit_id in player.units:
      var blocks = []
      var unit = player.units[unit_id]
      for i in range(unit.kind.width):
        blocks.append(0)
        blocks.append(i)
      # TODO jim: Different color for each unit.
      packing_data.append([Color(0, 1, 0), [x, 0], blocks])
      x += 1
    Armies.get_child(1 - pid).data = packing_data

func _ready():
  for unit in units:
    if units[unit].cost != null:
      var bar = unit_buy_bar.instance()
      bar.init(units[unit]);
      unit_bars[unit] = bar
      bar.connect("buy_unit", self, "_on_buy_unit", [unit])
      UnitList.add_child(bar)
  EndTurnButton.connect("button_down", self, "_on_EndTurnButton_pressed")
  update_ui()

func _on_EndTurnButton_pressed():
  g.man.simulate_battle(self, '_on_simulate_battle_complete')

func _on_simulate_battle_complete(result):
  # TODO: charles: display result in some way
  if g.man.get_view().is_game_over():
    get_tree().change_scene("res://scenes/game_over.tscn")
  else:
    update_ui()

func _on_buy_unit(unit):
  # TODO: charles: Don't actually make moves until the end of the turn;
  # instead, keep local list of units and draw army with [current units] plus
  # [queued units]. This will allow us to implement undo, and potentially
  # display queued units in a different way (e.g. faded)
  g.man.perform_action(BuildUnit.new(0, units[unit]))
  update_ui()
