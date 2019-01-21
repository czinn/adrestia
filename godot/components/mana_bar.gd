extends Control

onready var g = get_node('/root/global')

onready var mana_gem_template = $mana_gem_template

func _ready():
	mana_gem_template.visible = false
	remove_child(mana_gem_template)

func redraw(player, mp_left = null):
	if mp_left == null: mp_left = player.mp
	var mp_max = g.rules.get_mana_cap()

	if len(get_children()) != mp_max:
		g.clear_children(self)
		for i in range(mp_max):
			var mana_gem = mana_gem_template.duplicate()
			mana_gem.visible = true
			mana_gem.rect_position.x += 20 * (mp_max - i - 1)
			add_child(mana_gem)

	for i in range(mp_max):
		var mana_gem = get_child(mp_max - i - 1)
		if i < mp_left:     mana_gem.texture = load('res://art-built/mana-gem.png')
		elif i < player.mp: mana_gem.texture = load('res://art-built/mana-gem-potential.png')
		else:               mana_gem.texture = load('res://art-built/mana-gem-empty.png')
