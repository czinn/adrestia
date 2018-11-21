extends Node

onready var g = get_node('/root/global')
onready var book_grid = $ui/book_select/book_grid
onready var book_button_scene = preload('res://components/book_button.tscn')

func _ready():
  g.clear_children(book_grid)
  for book in g.rules.get_books().values():
    var book_button = book_button_scene.instance()
    book_button.book = book
    book_grid.add_child(book_button)
