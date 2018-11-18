extends Node

onready var g = get_node('/root/global')

func _ready():
  g.init_scene()