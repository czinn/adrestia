extends Node

func _ready():
	pass

func _on_button_play_pressed():
	get_tree().change_scene("res://scenes/resource_select.tscn")