extends Control

onready var g = get_node('/root/global')

onready var avatar = $avatar
export var flipped = false

func _ready():
	redraw()

func redraw():
	avatar.rect_scale.x = -1 if flipped else 1
