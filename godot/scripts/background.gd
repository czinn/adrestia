extends Sprite

var ofs = 0.0

func on_resize():
	var v = get_viewport_rect()
	self.region_rect = Rect2(v.position.x, v.position.y, v.size.x + 150, v.size.y + 150)

func _process(time):
	ofs += time * 60.0
	ofs -= floor(ofs / 150.0) * 150.0
	self.offset = Vector2(-150.0 + ofs, -150.0 + ofs)

func _ready():
	on_resize()
	get_tree().get_root().connect('size_changed', self, 'on_resize')
	set_process(true)
