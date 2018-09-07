extends Node

onready var g = get_node('/root/global')
const Unit_buy_bar = preload('res://components/unit_buy_bar.tscn')
const Action = preload('res://native/action.gdns')

onready var r_label = $MarginContainer/VBoxContainer/Toolbar/R
onready var g_label = $MarginContainer/VBoxContainer/Toolbar/G
onready var b_label = $MarginContainer/VBoxContainer/Toolbar/B
onready var c_label = $MarginContainer/VBoxContainer/Toolbar/Coins
onready var plus_tech_bar = $MarginContainer/VBoxContainer/PlusTechBar
onready var r_button = $MarginContainer/VBoxContainer/PlusTechBar/RButton
onready var g_button = $MarginContainer/VBoxContainer/PlusTechBar/GButton
onready var b_button = $MarginContainer/VBoxContainer/PlusTechBar/BButton
onready var end_turn_button = $MarginContainer/VBoxContainer/Toolbar/EndTurnButton
onready var unit_list = $MarginContainer/VBoxContainer/ScrollContainer/UnitList
onready var armies = $MarginContainer/VBoxContainer/ArmiesBox/Armies
onready var particles = $MarginContainer/VBoxContainer/ArmiesBox/Particles
onready var animation_player = $AnimationPlayer

var unit_bars = {}
var units_to_buy = []

func _ready():
  end_turn_button.connect('pressed', self, 'on_end_turn_button_pressed')
  r_button.connect('pressed', self, 'on_upgrade_tech', ['r'])
  g_button.connect('pressed', self, 'on_upgrade_tech', ['g'])
  b_button.connect('pressed', self, 'on_upgrade_tech', ['b'])
  animation_player.play('particle')
  print(g)
  update_unit_buttons()
  start_of_turn()

func start_of_turn():
  unit_bars = {}
  units_to_buy = []
  update_ui_labels()
  update_armies(g.man.get_view().get_players())
  unit_list.visible = false
  plus_tech_bar.visible = true
  end_turn_button.disabled = true

func on_upgrade_tech(color):
  var colour = g.Colour_.of_char(color)
  var action = Action.new()
  action.init_tech_colour(colour)
  g.man.perform_action(action)
  update_unit_buttons()
  update_ui_labels()
  unit_list.visible = true
  plus_tech_bar.visible = false
  end_turn_button.disabled = false

func coins_spent():
  var result = 0
  for unit_id in units_to_buy:
    result += g.rules.get_unit_kind(unit_id).get_cost()
  return result

# Populate the unit purchase list with units the player can afford.
func update_unit_buttons():
  var me = g.man.get_view().get_players()[0]
  for child in unit_list.get_children():
    child.queue_free()
  for unit_id in g.unit_kinds:
    var unit_kind = g.unit_kinds[unit_id]
    var tech = unit_kind.get_tech()
    if tech.not_null() and me.tech.includes(tech):
      var bar = Unit_buy_bar.instance()
      bar.init(unit_kind);
      unit_bars[unit_id] = bar
      bar.connect('buy_unit', self, 'on_buy_unit', [unit_id])
      unit_list.add_child(bar)

# Update UI labels.
func update_ui_labels():
  var me = g.man.get_view().get_players()[0]
  r_label.text = str(me.tech.red)
  g_label.text = str(me.tech.green)
  b_label.text = str(me.tech.blue)
  c_label.text = str(me.coins - coins_spent())
 
# Update armies. players: The armies to draw.
# This needs to be a separate function because when a battle happens, we first
# draw the armies before the battle.
func update_armies(players):
  for pid in range(players.size()):
    var player = players[pid]
    # jim: This triggers the redraw.
    armies.get_child(1 - pid).data = player.units

func on_buy_unit(unit_id):
  var me = g.man.get_view().get_players()[0]
  var cost = g.unit_kinds[unit_id].get_cost()
  if coins_spent() + cost <= me.coins:
    units_to_buy.append(unit_id)
  update_ui_labels()

func on_end_turn_button_pressed():
  end_turn_button.disabled = true
  var action = Action.new()
  action.init_units(units_to_buy)
  g.man.perform_action(action)
  g.man.end_turn(self, '_on_enemy_turn_done')

func _on_enemy_turn_done(battle):
  var animation = animation_player.get_animation('particle')

  var players = battle.get_players()
  var attacks = battle.get_attacks()

  update_armies(players)
  
  # TODO jim: Stagger attacks for effect.
  for attack in attacks:
    var spark = Sprite.new()
    spark.texture = load('res://art/attack.png')
    print(armies)

    var atk_poly = armies.get_child(1 - attack.from_player).polygons[attack.from_unit]
    var def_poly = armies.get_child(1 - attack.to_player).polygons[attack.to_unit]
    var def_unit = armies.get_child(1 - attack.to_player).data[attack.to_unit]

    # TODO jim: Stop spreading 50, and simple fractions and multiples thereof,
    # across the codebase
    var center_offset = Vector2(25, 25)

    # TODO jim: Previous attacks had a target_square_index; new attacks don't,
    # so for now we just assume it's 0. Perhaps we can make it uniform random.
    var tiles = def_unit.kind.get_tiles()
    print(def_unit.as_json().result)
    print(tiles)
    var square_x = tiles[0]
    var square_y = tiles[1]
    var square_offset = Vector2(50 * square_x, 50 * square_y)
    var start_scale = atk_poly.global_scale.x
    var final_scale = def_poly.global_scale.y
    var start_position = atk_poly.global_position + start_scale * center_offset
    var final_position = def_poly.global_position + final_scale * (center_offset + square_offset)
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
  var new_players = g.man.get_view().get_players()
  for attack in attacks:
    var def_poly = armies.get_child(1 - attack.to_player).polygons[attack.to_unit]
    var def_unit = armies.get_child(1 - attack.to_player).data[attack.to_unit]
    var health_icons = def_poly.get_node('UnitInfo/HealthIcons')

    # Determine new health of unit, if it still exists
    var new_units = new_players[attack.to_player].units
    var new_health = 0
    if new_units.has(attack.to_unit):
      new_health = new_units[attack.to_unit].health

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
  if g.man.get_view().get_winners():
    # TODO: charles: display result in some way
    get_tree().change_scene("res://scenes/game_over.tscn")
  else:
    end_turn_button.disabled = false
    start_of_turn()
