extends SceneTree

const Resources = preload("res://lib/resources.gd")

func _init():
  var r = Resources.of_json(JSON.parse('{"red": 3, "blue": 2}').result)
  assert(JSON.print(r.jsonify(), "", true) == '{"blue":2,"red":3}')
  quit()
