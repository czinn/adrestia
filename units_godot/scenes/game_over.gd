extends VBoxContainer

onready var g = get_node("/root/global")

onready var timer = $Timer
onready var label = $Label

func _ready():
  timer.connect("timeout", self, "timeout")
  if g.man.get_view().players[0].is_alive():
    label.text = 'Victory!'
  else:
    label.text = 'Defeat!'

func timeout():
  get_tree().change_scene("res://scenes/title.tscn")
