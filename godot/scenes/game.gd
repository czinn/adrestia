extends Node

# Book Select.
# (Maybe this should be a separate scene that we instantiate.

onready var g = get_node('/root/global')
onready var book_select = $ui/book_select
onready var book_grid = $ui/book_select/book_grid
onready var play_button = $ui/book_select/play_button
onready var book_button_scene = preload('res://components/book_button.tscn')

onready var spell_select = $ui/spell_select
onready var book_tabs = $ui/spell_select/book_tabs
onready var spell_list_scene = preload('res://components/spell_list.tscn')

func _ready():
  print('ain nobody bring us')
  g.clear_children(book_grid)
  for book in g.rules.get_books().values():
    var book_button = book_button_scene.instance()
    book_grid.add_child(book_button)
    book_button.book = book
    book_button.button.connect('pressed', self, 'on_toggle_book', [book_button])
  print(play_button)
  play_button.connect('pressed', self, 'on_choose_tech')
  book_select.visible = true
  spell_select.visible = false

func get_selected_books():
  var result = []
  for book_button in book_grid.get_children():
    if book_button.checked:
      result.append(book_button.book)
  return result

func on_toggle_book(book_button):
  var already_checked = book_button.checked
  if already_checked or len(get_selected_books()) < 3:
    book_button.checked = not already_checked

func on_choose_tech():
  var selected_books = get_selected_books()
  if len(selected_books) < 1 or len(selected_books) > 3:
    return
  var selected_book_ids = g.map_method(selected_books, 'get_id')
  g.state = g.GameState.new()
  g.state.init(g.rules, [selected_book_ids, ['conjuration']])
  book_select.visible = false
  spell_select.visible = true
  print(g.state.as_json().result)

  g.clear_children(book_tabs)
  
  # not needed but just in case
  selected_books = g.state.players[0].books
  for i in range(len(selected_books)):
    var book = selected_books[i]
    var tab = NinePatchRect.new()
    tab.name = book.get_name()
    var spell_list = spell_list_scene.instance()
    spell_list.book = book
    tab.add_child(spell_list)
    book_tabs.add_child(tab)
    #book_tabs.set_tab_title(i, book.get_name())
    book_tabs.set_tab_title(1, 'weh')
    #book_tabs.set_tab_icon(i, load('res://art-built/book/conjuration.png'))
