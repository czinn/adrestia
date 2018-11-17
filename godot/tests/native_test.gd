extends SceneTree

# Tests the integration of gdnative modules.
# Also serves as an example of how exactly to use them.

const Book = preload('res://native/book.gdns')

func _init():
	print('Begin tests')
	var b = Book.new()
	b.load_json_string('{"id": "conjuration", "name": "Book of Conjuration", "spells": ["yes", "no", "maybe_so"]}')
	print(b.get_id())
	print(b.get_name())
	print(b.get_spells())
	print(b.get_spells()[1])
	quit()
