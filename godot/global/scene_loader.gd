extends Node

# A bit heavy-handed, but opens up the possibility for sick scene-loading
# transitions.

# See
# http://docs.godotengine.org/en/latest/tutorials/io/background_loading.html

onready var g = get_node('/root/global')
onready var root = get_tree().get_root()
const transition_scene = preload('res://components/transition_bg.tscn')
var scene_holder
var transition
var loader
var current_scene

func _ready():
	transition = transition_scene.instance()
	current_scene = root.get_child(root.get_child_count() - 1)
	scene_holder = Node.new()
	add_child(scene_holder)
	call_deferred('possess_initial_scene')

func possess_initial_scene():
	root.remove_child(current_scene)
	scene_holder.add_child(current_scene)

func _process(time):
	# For scene loader.
	if loader == null:
		set_process(false)
		return

	# Busy loop for a frame (maybe this is bad)
	var t = OS.get_ticks_msec()
	while OS.get_ticks_msec() < t + 16:
		var err = loader.poll()
		if err == ERR_FILE_EOF: # Load finished.
			# Don't advance scene until animation is done.
			var resource = loader.get_resource()
			loader = null
			set_new_scene(resource)
			break
		elif err == OK:
			update_progress()
		else:
			show_error()
			loader = null
			break

func goto_scene(scene_name):
	g.close_tooltip()
	loader = ResourceLoader.load_interactive('res://scenes/%s.tscn' % [scene_name])
	set_process(true)
	transition.visible = true
	root.add_child(transition)
	transition.animation_player.play('slide_in')

func update_progress():
	var progress = float(loader.get_stage()) / loader.get_stage_count()

func set_new_scene(scene_resource):
	if transition and transition.animation_player and transition.animation_player.is_playing():
		yield(transition.animation_player, 'animation_finished')

	current_scene.queue_free()
	current_scene = scene_resource.instance()
	scene_holder.add_child(current_scene)

	transition.animation_player.play('slide_out')
	yield(transition.animation_player, 'animation_finished')
	root.remove_child(transition)
