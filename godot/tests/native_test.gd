extends SceneTree

# Tests the integration of gdnative modules.
# Also serves as an example of how exactly to use them.

const Book = preload('res://native/book.gdns')
const Duration = preload('res://native/duration.gdns')
const DurationUnit = preload('res://native/duration_unit.gd')
const StickyInvoker = preload('res://native/sticky_invoker.gdns')

func _init():
	print('test Book')
	var b = Book.new()
	b.load_json_string('{"id": "conjuration", "name": "Book of Conjuration", "spells": ["yes", "no", "maybe_so"]}')
	print(b.get_id() == "conjuration")
	print(b.get_name() == "Book of Conjuration")
	print(b.get_spells().size() == 3)
	print(b.get_spells()[1] == "no")
	print('test Duration')
	var d = Duration.new()
	d.load_json_string('{"turns": 5}')
	print(d.get_unit() == DurationUnit.TURNS)
	print(d.get_value() == 5)
	d.subtract_step()
	d.subtract_turn()
	print(d.get_value() == 4)
	print('test StickyInvoker')
	var si = StickyInvoker.new()
	si.load_json_string('{"sticky_id": "shield", "amount": 6, "duration": {"steps": 1}}')
	d = si.get_duration()
	print(d.get_value() == 1)
	print(d.get_unit() == DurationUnit.STEPS)
	print(si.get_sticky_id() == "shield")
	quit()
