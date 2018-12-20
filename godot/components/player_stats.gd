extends Control

onready var g = get_node('/root/global')

onready var hp_label = $hp_label
onready var mp_label = $mp_label
onready var mp_icons = $mp_icons
onready var mp_icon_template = $mp_icon_template

var old_hp = null
var old_mp = null

func _ready():
	mp_icon_template.visible = false

func redraw(player, mp_override = null):
	if hp_label == null: return
	hp_label.text = '%d/%d' % [player.hp, player.max_hp]
	if old_hp != null && old_hp != player.hp:
		g.summon_delta(hp_label, player.hp - old_hp, Color(1, 0.02, 0.29))
	old_hp = player.hp

	var mp_left = player.mp
	if mp_override != null && mp_override != mp_left:
		mp_left = mp_override
	else:
		if old_mp != null && old_mp != player.mp:
			g.summon_delta(mp_label, player.mp - old_mp, Color(0.27, 0.55, 1))
		old_mp = player.mp
	
	var mp_max = g.rules.get_mana_cap()
	if len(mp_icons.get_children()) != mp_max:
		g.clear_children(mp_icons)
		for i in range(mp_max):
			var mp_icon = mp_icon_template.duplicate()
			mp_icon.visible = true
			mp_icon.rect_position.x += 20 * (mp_max - i - 1)
			mp_icons.add_child(mp_icon)
	for i in range(mp_max):
		var mp_icon = mp_icons.get_child(mp_max - i - 1)
		if i < mp_left:
			mp_icon.texture = load('res://art-built/mana-gem.png')
		elif i < mp_left + player.mp_regen:
			mp_icon.texture = load('res://art-built/mana-gem-potential.png')
		else:
			mp_icon.texture = load('res://art-built/mana-gem-empty.png')
	
	mp_label.text = '%d/%d (+%d)' % [mp_left, mp_max, player.mp_regen]
