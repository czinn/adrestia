extends Control

onready var g = get_node('/root/global')

onready var animation_player = $animation_player
onready var avatar = $avatar
export var flipped = false

func _ready():
	redraw()
	animation_player.play('head_bounce')

func redraw():
	avatar.rect_scale.x = -1 if flipped else 1
