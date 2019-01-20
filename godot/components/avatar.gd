extends Control

onready var avatar = $avatar

export var flipped = false setget set_flipped

func _ready():
	set_flipped(flipped)

func set_flipped(flipped_):
	flipped = flipped_
	if avatar == null: return
	avatar.rect_scale.x = -1 if flipped else 1
