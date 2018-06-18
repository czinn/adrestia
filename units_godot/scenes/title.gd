extends Node

onready var g = get_node("/root/global")

func _ready():
  get_tree().set_auto_accept_quit(true)
  get_tree().set_quit_on_go_back(true)

func _on_button_play_pressed():
  g.new_ai_game()
