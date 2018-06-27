extends Node

var unit_buy_bar = preload('res://components/unit_buy_bar.tscn')
var BuildUnit = preload('res://lib/actions/build_unit.gd')

onready var g = get_node("/root/global")
onready var units = get_node("/root/UnitKinds").units

onready var r_label = $MarginContainer/VBoxContainer/Toolbar/R
onready var g_label = $MarginContainer/VBoxContainer/Toolbar/G
onready var b_label = $MarginContainer/VBoxContainer/Toolbar/B
onready var EndTurnButton = $MarginContainer/VBoxContainer/Toolbar/EndTurnButton
onready var UnitList = $MarginContainer/VBoxContainer/ScrollContainer/UnitList
onready var Armies = $MarginContainer/VBoxContainer/ArmiesBox/Armies
onready var particles = $MarginContainer/VBoxContainer/ArmiesBox/Particles
onready var animation_player = $AnimationPlayer
var unit_bars = {}

func update_ui():
  var view = g.man.get_view()

  r_label.text = str(view.players[0].resources.r)
  g_label.text = str(view.players[0].resources.g)
  b_label.text = str(view.players[0].resources.b)
  for unit in unit_bars:
    unit_bars[unit].buy_button.disabled = !view.players[0].resources.subsumes(units[unit].cost)
  
  for pid in range(view.players.size()):
    var player = view.players[pid]
    Armies.get_child(1 - pid).data = player.units.values()

func _ready():
  for unit in units:
    if units[unit].cost != null:
      var bar = unit_buy_bar.instance()
      bar.init(units[unit]);
      unit_bars[unit] = bar
      bar.connect("buy_unit", self, "_on_buy_unit", [unit])
      UnitList.add_child(bar)
  EndTurnButton.connect("button_down", self, "_on_EndTurnButton_pressed")
  animation_player.play('particle')
  update_ui()

func _on_EndTurnButton_pressed():
  g.man.end_turn(self, '_on_enemy_turn_done')

func _on_enemy_turn_done():
  update_ui()

  # wait for 0.5s
  var t = Timer.new()
  t.set_wait_time(0.5)
  t.set_one_shot(true)
  self.add_child(t)
  t.start()
  yield(t, 'timeout')
  t.queue_free()

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
