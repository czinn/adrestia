extends SceneTree

const ArmyView = preload('res://components/army_view.gd')

func _init():
  # .#.#
  # ####
  # ###.
  var square_coords = [0,1, 0,2, 1,0, 1,1, 1,2, 2,1, 2,2, 3,0, 3,1]
  var poly_coords = ArmyView.squares_to_polygon(square_coords)
  print(poly_coords)
  assert(poly_coords == [[1, 0], [1, 1], [0, 1], [0, 3], [3, 3], [3, 2], [4, 2], [4, 0], [3, 0], [3, 1], [2, 1], [2, 0]])
  for [x,y] in poly_coords:
    print([x, y])
  quit()
