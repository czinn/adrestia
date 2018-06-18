extends VBoxContainer

var unit_buy_bar = preload('res://components/unit_buy_bar.tscn')
var BuildUnit = preload('res://lib/actions/build_unit.gd')

onready var r_label = $Toolbar/R
onready var g_label = $Toolbar/G
onready var b_label = $Toolbar/B
onready var EndTurnButton = $Toolbar/EndTurnButton
onready var UnitList = $ScrollContainer/UnitList
onready var g = get_node("/root/global")
onready var units = get_node("/root/UnitKinds").units
var unit_bars = {}

func update_ui():
  r_label.text = str(g.gs.players[0].resources.r)
  g_label.text = str(g.gs.players[0].resources.g)
  b_label.text = str(g.gs.players[0].resources.b)
  for unit in unit_bars:
    unit_bars[unit].buy_button.disabled = !g.gs.players[0].resources.subsumes(units[unit].cost)

func _ready():
  print(units.size())
  for unit in units:
    if units[unit].cost != null:
      var bar = unit_buy_bar.instance()
      bar.init(units[unit]);
      unit_bars[unit] = bar
      bar.connect("buy_unit", self, "_on_buy_unit", [unit])
      UnitList.add_child(bar)
  EndTurnButton.connect("button_down", self, "_on_EndTurnButton_pressed")
  g.gs.start_game()
  update_ui()

func _on_EndTurnButton_pressed():
  g.gs.simulate_battle()
  if g.gs.is_game_over():
    get_tree().change_scene("res://scenes/game_over.tscn")
  else:
    update_ui()

func _on_buy_unit(unit):
  print("buying unit in game.gd")
  g.gs.perform_action(BuildUnit.new(0, units[unit]))
  update_ui()
