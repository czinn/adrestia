extends Sprite

var ofs = Vector2(0.0, 0.0)
var texture_size = Vector2(1.0, 1.0)

func on_resize():
	var v = get_viewport_rect()
	self.region_rect = Rect2(v.position.x, v.position.y, v.size.x + texture_size.x, v.size.y + texture_size.y)

func _process(time):
	ofs += Vector2(time * 60.0, time * 60.0)
	while ofs.x >= texture_size.x: ofs.x -= texture_size.x
	while ofs.y >= texture_size.y: ofs.y -= texture_size.y
	self.offset = -texture_size + ofs

func _ready():
	texture_size = self.texture.get_size()
	on_resize()
	get_tree().get_root().connect('size_changed', self, 'on_resize')
	set_process(true)
