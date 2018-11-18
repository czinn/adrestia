extends Node

func _ready():
  pass

# Call from _ready() in every scene to set up the nice critical-rectangle-centred camera.
func init_scene():
  get_tree().get_root().connect('size_changed', self, 'on_size_changed')
  on_size_changed()

func on_size_changed():
  var screen_size = OS.get_real_window_size()
  
  var screen_w = screen_size.x
  var screen_h = screen_size.y
  var screen_ratio = 1.0 * screen_w / screen_h
  
  var project_w = ProjectSettings.get_setting('display/window/size/width')
  var project_h = ProjectSettings.get_setting('display/window/size/height')
  var project_ratio = 1.0 * project_w / project_h
  
  var offset
  var view_rect
  if project_ratio < screen_ratio:
    # Wide screen.
    var project_effective_w = project_w * screen_ratio / project_ratio
    offset = Vector2((project_effective_w - project_w) / 2 , 0)
    view_rect = Vector2(project_effective_w, project_h)
  else:
    # Tall screen.
    var project_effective_h = project_h * project_ratio / screen_ratio
    offset = Vector2(0, (project_effective_h - project_h) / 2)
    view_rect = Vector2(project_w, project_effective_h)
  
  var root = get_tree().get_root()
  root.canvas_transform = Transform2D(Vector2(1, 0), Vector2(0, 1), offset)
  root.set_size_override(true, view_rect)