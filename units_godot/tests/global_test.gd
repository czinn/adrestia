extends SceneTree

# Test for wrappers at the global level.

const Action = preload('res://native/action.gdns')
const Global = preload('res://global.gd')

var g

func _init():
  g = Global.new()
  var red = g.Colour_.of_char('r')
  var action = Action.new()
  action.init_tech_colour(red)
  g.man.perform_action(action)
  quit()
