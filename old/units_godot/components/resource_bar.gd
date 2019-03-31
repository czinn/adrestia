tool
extends Control

var total = 7
var value = 3
var border_color = Color(0, 0, 0)
var fill_color = Color(1, 0, 0)
var bg_color = Color(1, 1, 1)

func _draw():
  var s = rect_size
  for i in range(total):
    var fill = fill_color if total - i <= value else bg_color
    draw_rect(Rect2(0, i * s.y / total, s.x, s.y / total), fill, true)
    draw_rect(Rect2(0, i * s.y / total, s.x, s.y / total), border_color, false)

func set_value(v):
  value = v
  update()

func set_total(t):
  total = t
  update()