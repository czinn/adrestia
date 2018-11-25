extends Control

onready var sticky_display_scene = preload('res://components/sticky_display.tscn')

onready var g = get_node('/root/global')

onready var hp_label = $hp_label
onready var mp_label = $mp_label
onready var stickies = $stickies/hbox

func _ready():
	pass

func redraw(player, mp_override = null):
	if hp_label == null: return
	hp_label.text = '%d/%d' % [player.hp, player.max_hp]
	var mp_left = player.mp
	if mp_override != null:
		mp_left = mp_override
	mp_label.text = '%d/%d (+%d)' % [mp_left, g.rules.get_mana_cap(), player.mp_regen]
	g.clear_children(stickies)
	for sticky in player.stickies:
		var sticky_display = sticky_display_scene.instance()
		stickies.add_child(sticky_display)
		print(sticky.amount)
		sticky_display.redraw(sticky)
