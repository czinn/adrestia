extends Control

onready var sticky_display_scene = preload('res://components/sticky_display.tscn')

onready var g = get_node('/root/global')

onready var stickies_hbox = $scroller/hbox

func _ready():
	g.clear_children(stickies_hbox)

func redraw(stickies):
	if stickies_hbox == null: return
	g.clear_children(stickies_hbox)
	for sticky in stickies:
		var sticky_display = sticky_display_scene.instance()
		sticky_display.sticky = sticky
		stickies_hbox.add_child(sticky_display)
		sticky_display.redraw()
