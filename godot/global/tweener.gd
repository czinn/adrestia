extends Object

signal done

var root

func _init(root_):
	root = root_

func tween_to(thing, destination, duration):
	var animator = AnimationPlayer.new()
	root.add_child(animator)
	var anim = Animation.new()
	anim.length = duration
	var thing_path = str(thing.get_path())
	var track_path = '%s:rect_position' % thing_path
	var animation_name = thing_path.replace('/', '-')
	anim.add_track(Animation.TYPE_VALUE, 0)
	anim.track_set_path(0, track_path)
	anim.track_set_interpolation_type(0, Animation.INTERPOLATION_CUBIC)
	anim.track_insert_key(0, 0.0, thing.rect_position)
	anim.track_insert_key(0, duration, destination)
	animator.add_animation(animation_name, anim)
	animator.play(animation_name)
	yield(animator, 'animation_finished')
	animator.queue_free()
	emit_signal('done')
