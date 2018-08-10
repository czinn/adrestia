extends Node

var unit_buy_bar = preload('res://components/unit_buy_bar.tscn')
var BuildUnit = preload('res://lib/actions/build_unit.gd')

onready var g = get_node("/root/global")
onready var units = get_node("/root/UnitKinds").units

onready var r_label = $MarginContainer/VBoxContainer/Toolbar/R
onready var g_label = $MarginContainer/VBoxContainer/Toolbar/G
onready var b_label = $MarginContainer/VBoxContainer/Toolbar/B
onready var c_label = $MarginContainer/VBoxContainer/Toolbar/Coins
onready var end_turn_button = $MarginContainer/VBoxContainer/Toolbar/EndTurnButton
onready var unit_list = $MarginContainer/VBoxContainer/ScrollContainer/UnitList
onready var armies = $MarginContainer/VBoxContainer/ArmiesBox/Armies
onready var particles = $MarginContainer/VBoxContainer/ArmiesBox/Particles
onready var animation_player = $AnimationPlayer
var unit_bars = {}

func update_ui():
  var view = g.man.get_view()

  r_label.text = str(view.players[0].resources.r)
  g_label.text = str(view.players[0].resources.g)
  b_label.text = str(view.players[0].resources.b)
  c_label.text = "0"
  for unit in unit_bars:
    unit_bars[unit].buy_button.disabled = !view.players[0].resources.subsumes(units[unit].cost)
  
  for pid in range(view.players.size()):
    var player = view.players[pid]
    # jim: This triggers the redraw. duplicate() is needed because when drawing
    # attack particles, we need access to what is CURRENTLY drawn on the
    # screen, even if the actual game state has changed in the meantime.
    armies.get_child(1 - pid).data = player.units.duplicate()

func _ready():
  for unit in units:
    if units[unit].cost != null:
      var bar = unit_buy_bar.instance()
      bar.init(units[unit]);
      unit_bars[unit] = bar
      bar.connect("buy_unit", self, "_on_buy_unit", [unit])
      unit_list.add_child(bar)
  end_turn_button.connect("pressed", self, "_on_end_turn_button_pressed")
  animation_player.play('particle')
  update_ui()

func _on_end_turn_button_pressed():
  end_turn_button.disabled = true
  g.man.end_turn(self, '_on_enemy_turn_done')

func _on_enemy_turn_done():
  update_ui()
  g.man.simulate_battle(self, '_on_simulate_battle_complete')

func _on_simulate_battle_complete(result):
  var gs = g.man.get_view()
  var animation = animation_player.get_animation('particle')
  
  # TODO jim: Stagger attacks for effect.
  for attack in result.attacks:
    var spark = Sprite.new()
    spark.texture = load('res://art/attack.png')
    var attacking_unit = armies.get_child(1 - attack.player).data[attack.unit_id]
    var defending_unit = armies.get_child(1 - attack.target_player).data[attack.target_unit_id]

    # TODO jim: Stop spreading 50, and simple fractions and multiples thereof,
    # across the codebase
    var center_offset = Vector2(25, 25)
    var square_x = defending_unit.kind.tiles[2 * attack.target_square_index]
    var square_y = defending_unit.kind.tiles[2 * attack.target_square_index + 1]
    var square_offset = Vector2(50 * square_x, 50 * square_y)
    var start_scale = attacking_unit.polygon.global_scale.x
    var final_scale = defending_unit.polygon.global_scale.y
    var start_position = attacking_unit.polygon.global_position + start_scale * center_offset
    var final_position = defending_unit.polygon.global_position + final_scale * (center_offset + square_offset)
    particles.add_child(spark)

    var track_id = animation.add_track(Animation.TYPE_VALUE)
    var path = NodePath("%s:position" % spark.get_path())
    animation.track_set_path(track_id, path)

    var offset = square_offset + center_offset
    animation.track_insert_key(track_id, 0.0, start_position)
    animation.track_insert_key(track_id, 1.0, final_position)

  animation_player.play('particle')

  var t = Timer.new()
  t.set_wait_time(1.0)
  t.set_one_shot(true)
  self.add_child(t)
  t.start()
  yield(t, 'timeout')
  t.queue_free()

  animation.clear()
  for spark in particles.get_children():
    spark.queue_free()

  # Update hearts immediately, so that it looks like the spark "took away" the heart.
  for attack in result.attacks:
    var defending_unit = armies.get_child(1 - attack.target_player).data[attack.target_unit_id]
    var health_icons = defending_unit.polygon.get_node('UnitInfo/HealthIcons')

    # Determine new health of unit, if it still exists
    var new_units = gs.players[attack.target_player].units
    var new_health = 0
    if new_units.has(attack.target_unit_id):
      new_health = new_units[attack.target_unit_id].health

    for i in range(health_icons.get_child_count()):
      var icon = health_icons.get_child(i)
      if i >= new_health:
        icon.texture = load('res://art/heart-empty.png')

  t = Timer.new()
  t.set_wait_time(0.5)
  self.add_child(t)
  t.set_paused(false)
  t.start()
  yield(t, 'timeout')
  t.queue_free()

  end_turn_button.disabled = false
  if g.man.get_view().is_game_over():
    # TODO: charles: display result in some way
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
