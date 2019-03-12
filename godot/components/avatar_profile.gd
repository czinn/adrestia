extends Control

onready var g = get_node('/root/global')

onready var avatar = $avatar
onready var name_label = $name_label
onready var fc_label = $fc_label
onready var wins_label = $wins_label
onready var online_label = $online_label
onready var button1 = $texture_button1
onready var button2 = $texture_button2

func _ready():
	name_label.text = ''
	fc_label.text = ''
	wins_label.text = ''
	online_label.text = ''

func redraw():
	pass
