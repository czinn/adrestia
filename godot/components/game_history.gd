extends Control

onready var g = get_node('/root/global')

var history = [] setget set_history
var health_history = [] setget set_health_history
var view_player_id = 0 setget set_view_player_id

onready var vbox = $scroll_container/vbox
onready var template_row = $template_row

func _ready():
	template_row.visible = false
	redraw()

func set_history(history_):
	history = history_
	redraw()

func set_health_history(health_history_):
	health_history = health_history_
	redraw()

func set_view_player_id(view_player_id_):
	view_player_id = view_player_id_
	redraw()

func health_helper(health_node, index, player):
	if index >= len(health_history):
		health_node.visible = false
		return
	health_node.visible = true
	var label = g.child(health_node, 'label')
	var delta_label = g.child(health_node, 'delta_label')
	label.text = str(health_history[index][player])
	if index + 1 < len(health_history):
		var delta = max(0, health_history[index + 1][player]) - health_history[index][player]
		delta_label.text = ('+' if delta >= 0 else '') + str(delta)

func redraw():
	if vbox == null: return

	g.clear_children(vbox)
	for index in range(len(history)):
		var turn = template_row.duplicate()
		var my_spell_list = g.child(turn, 'my_spell_list')
		var enemy_spell_list = g.child(turn, 'enemy_spell_list')
		var turn_label = g.child(turn, 'turn_label')
		var my_health = g.child(turn, 'my_health')
		var enemy_health = g.child(turn, 'enemy_health')
		health_helper(my_health, index, view_player_id)
		health_helper(enemy_health, index, 1 - view_player_id)
		my_spell_list.immediately_show_tooltip = true
		enemy_spell_list.immediately_show_tooltip = true
		my_spell_list.spells = history[index][view_player_id]
		enemy_spell_list.spells = history[index][1 - view_player_id]
		turn_label.text = 'Turn ' + str(index + 1)
		turn.visible = true
		vbox.add_child(turn)
