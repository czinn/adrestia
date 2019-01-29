extends Control

onready var g = get_node('/root/global')

var history = [] setget set_history
var view_player_id = 0 setget set_view_player_id

onready var vbox = $scroll_container/vbox
onready var template_row = $template_row
onready var template_spacer = $template_spacer

func _ready():
	template_row.visible = false
	redraw()

func set_history(history_):
	history = history_
	redraw()

func set_view_player_id(view_player_id_):
	view_player_id = view_player_id_
	redraw()

func redraw():
	if vbox == null: return

	g.clear_children(vbox)
	vbox.add_child(template_spacer.duplicate())
	for index in range(len(history)):
		var turn = template_row.duplicate()
		var my_spell_list = g.child(turn, 'my_spell_list')
		var enemy_spell_list = g.child(turn, 'enemy_spell_list')
		var turn_label = g.child(turn, 'turn_label')
		my_spell_list.spells = history[index][view_player_id]
		enemy_spell_list.spells = history[index][1 - view_player_id]
		turn_label.text = 'Turn ' + str(index + 1)
		turn.visible = true
		vbox.add_child(turn)
	vbox.add_child(template_spacer.duplicate())
