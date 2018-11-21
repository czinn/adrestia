extends Node

# See
# http://docs.godotengine.org/en/latest/tutorials/io/background_loading.html

var loader
var current_scene

func _ready():
  var root = get_tree().get_root()
  current_scene = root.get_child(root.get_child_count() - 1)
  pass

func _process(time):
  # For scene loader.
  if loader == null:
    set_process(false)
    return

  var t = OS.get_ticks_msec()
  while OS.get_ticks_msec() < t + 100:
    var err = loader.poll()
    if err == ERR_FILE_EOF: # Load finished.
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

func goto_scene(path):
  loader = ResourceLoader.load_interactive('res://scenes/game.tscn')
  set_process(true)
  current_scene.queue_free()

func update_progress():
  var progress = float(loader.get_stage()) / loader.get_stage_count()

func set_new_scene(scene_resource):
  current_scene = scene_resource.instance()
  get_node('/root').add_child(current_scene)
