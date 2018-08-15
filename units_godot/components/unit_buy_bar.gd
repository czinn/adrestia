extends Control

signal buy_unit

var unit_kind

onready var hbox = $HBox
onready var image = $HBox/Image
onready var unit_name = $HBox/Center/UnitName
onready var unit_width = $HBox/Center/UnitDesc/WidthDesc/UnitWidth
onready var unit_health = $HBox/Center/UnitDesc/HealthDesc/UnitHealth
onready var unit_attack = $HBox/Center/UnitDesc/AttackDesc/UnitAttack
onready var buy_button = $HBox/BuyButton

func init(unit_kind_):
  unit_kind = unit_kind_

func _ready():
  # TODO: jim: Load textures from string
  image.texture = load(unit_kind.get_image())
  var unit_text = unit_kind.get_name()
  unit_name.text = "%s (%d)" % [unit_kind.get_name(), unit_kind.get_cost()]
  
  unit_width.text = str(unit_kind.get_width())
  unit_health.text = str(unit_kind.get_health())
  unit_attack.text = str(unit_kind.get_attack())

func _on_BuyButton_pressed():
  emit_signal("buy_unit")
