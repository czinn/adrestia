extends Control

onready var g = get_node('/root/global')

onready var avatar = $avatar
onready var hp_label = $hp_label
onready var mp_label = $mp_label
onready var mp_regen_label = $mp_regen_label

export var flipped = false

var old_hp = null
var old_mp = null

func _ready():
	pass

func redraw(player, mp_left = null):
	if hp_label == null: return
	if mp_left == null: mp_left = player.mp

	avatar.rect_scale.x = -1 if flipped else 1

	hp_label.text = str(max(0, player.hp))
	if old_hp != null && old_hp != player.hp:
		g.summon_delta(hp_label, player.hp - old_hp, Color(1, 0.02, 0.29))
	old_hp = player.hp

	if old_mp != null && old_mp != player.mp:
		g.summon_delta(mp_label, player.mp - old_mp, Color(0.27, 0.55, 1))
	old_mp = player.mp

	var mp_max = g.rules.get_mana_cap()

	mp_label.text = str(mp_left)
	mp_regen_label.text = '+%s' % [player.mp_regen]
