extends SceneTree

# Tests the integration of gdnative modules.
# Also serves as an example of how exactly to use them.

const Book = preload('res://native/book.gdns')
const Duration = preload('res://native/duration.gdns')
const DurationUnit = preload('res://native/duration_unit.gd')

func _init():
	print('Test Book')
	var b = Book.new()
	b.load_json_string('{"id": "conjuration", "name": "Book of Conjuration", "spells": ["yes", "no", "maybe_so"]}')
	print(b.get_id())
	print(b.get_name())
	print(b.get_spells())
	print(b.get_spells()[1])
	var d = Duration.new()
	d.load_json_string('{"game": true}')
	print(d.get_unit() == DurationUnit.FULL_GAME)
	print(d.get_value())
	d.subtract_step()
	d.subtract_turn()
	print(d.get_value())
	quit()
