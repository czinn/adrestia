extends Control

onready var g = get_node('/root/global')
onready var hp_label = $hp_label
onready var mp_label = $mp_label

func _ready():
	pass

func redraw(player, mp_override = null):
	if hp_label == null: return
	hp_label.text = '%d/%d' % [player.hp, player.max_hp]
	var mp_left = player.mp
	if mp_override != null:
		mp_left = mp_override
	mp_label.text = '%d/%d (+%d)' % [mp_left, g.rules.get_mana_cap(), player.mp_regen]
