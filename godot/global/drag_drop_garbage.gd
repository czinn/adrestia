# jim: unused code from attempt at true drag-and-drop... keeping around for
# later reference

func _input(event):
	if event is InputEventMouseButton and event.button_index == BUTTON_LEFT and not event.pressed:
		var book_image = g.drag_drop.end_drag()
		if book_image:
			on_end_drag_book(book_image)

func on_start_drag_book(book_button):
	var image = g.drag_drop.clone_image(book_button.button)
	g.drag_drop.start_drag(book_button.button, book_button)
	book_button.button.texture_normal = book_placeholder_texture

func on_end_drag_book(book_image):
	var button = g.drag_drop.payload
	var book = button.book
	var i = chosen_books.find(null)
	if g.drag_drop.dragged_node_end.y > spells_panel.get_global_position().y or chosen_books.find(book) >= 0 or i == -1:
		button.button.texture_normal = book_image.texture
		book_image.queue_free()
		return
	chosen_books[i] = book
	var chosen_book_icon = chosen_books_vbox.get_child(i)
	yield(g.tween(book_image, chosen_book_icon.get_global_position(), 0.3), 'done')
	book_image.queue_free()
	print(book.get_name())
	chosen_books_vbox.get_child(i).texture_normal = g.get_book_texture(book.get_id())
