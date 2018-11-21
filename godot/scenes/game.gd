extends Node

onready var g = get_node('/root/global')
onready var book_grid = $ui/book_select/book_grid
onready var book_button_scene = preload('res://components/book_button.tscn')

func _ready():
  g.clear_children(book_grid)
  for book in g.rules.get_books().values():
    var book_button = book_button_scene.instance()
    book_grid.add_child(book_button)
    book_button.book = book
    book_button.button.connect('pressed', self, 'toggle_book', [book_button])

func selected_books():
  var result = []
  for book_button in book_grid.get_children():
    if book_button.checked:
      result.append(book_button.book)
  return result

func toggle_book(book_button):
  book_button.set_checked(not book_button.checked)
  print(selected_books())
