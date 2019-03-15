extends Control

onready var g = get_node('/root/global')

onready var avatar = $avatar
onready var name_label = $name_label
onready var fc_label = $fc_label
onready var wins_label = $wins_label
onready var online_label = $online_label
onready var buttons = $buttons
onready var challenge_button = $buttons/challenge

func _ready():
	name_label.text = ''
	fc_label.text = ''
	wins_label.text = ''
	online_label.text = ''
	buttons.visible = false

func redraw():
	pass
