extends Control

onready var sticky_display_scene = preload('res://components/sticky_display.tscn')

onready var g = get_node('/root/global')

onready var stickies_hbox = $grid

var sticky_displays = []

func _ready():
	g.clear_children(stickies_hbox)

func redraw(stickies):
	if stickies_hbox == null: return
	g.clear_children(stickies_hbox)
	sticky_displays = []
	for sticky in stickies:
		var sticky_display = sticky_display_scene.instance()
		sticky_display.sticky = sticky
		stickies_hbox.add_child(sticky_display)
		sticky_display.appear()
		sticky_displays.append(sticky_display)

func flash_sticky(index):
	if stickies_hbox == null: return
	sticky_displays[index].flash()

# Like redraw, but assumes that the only change is appending the last sticky
func redraw_append(stickies):
	if stickies_hbox == null: return
	var sticky = stickies[-1]
	var sticky_display = sticky_display_scene.instance()
	sticky_display.sticky = sticky
	stickies_hbox.add_child(sticky_display)
	sticky_display.fadein()
	sticky_displays.append(sticky_display)

# Like redraw, but assumes the only change is to the amounts or durations
# of the stickies.
func redraw_update(stickies):
	if stickies_hbox == null: return
	for i in range(len(sticky_displays)):
		sticky_displays[i].sticky = stickies[i]
		sticky_displays[i].redraw()

# Like redraw, but assumes that the only change is removing the
# sticky at the given index
func redraw_remove(index):
	if stickies_hbox == null: return
	sticky_displays[index].fadeout()
	sticky_displays.remove(index)