tool
extends Polygon2D

var Colour = preload('res://native/colour.gdns')

export(Color) var outline_color = Color(0,0,0) setget set_color
export(float) var outline_width = 2.0 setget set_outline_width

var unit

# TODO jim: UnitPolygon should handle more of its own drawing. Right now, most
# of it is done by army_view.gd.
func _init(unit_):
  unit = unit_

func _ready():
  color = unit.kind.get_colour().to_color().lightened(0.6)

func _draw():
    var poly = get_polygon()
    for i in range(1, poly.size()):
        draw_line(poly[i-1], poly[i], outline_color, outline_width)
    draw_line(poly[poly.size() - 1], poly[0], outline_color, outline_width)

func set_color(color):
    outline_color = color
    update()

func set_outline_width(new_outline_width):
    outline_width = new_outline_width
    update()
