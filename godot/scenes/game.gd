extends Node

# Book Select.
# (Maybe this should be a separate scene that we instantiate.

onready var g = get_node('/root/global')
onready var book_select = $ui/book_select
onready var book_grid = $ui/book_select/book_grid
onready var play_button = $ui/book_select/play_button

onready var book_button_scene = preload('res://components/book_button.tscn')
onready var spell_list_scene = preload('res://components/spell_list.tscn')
onready var spell_button_scene = preload('res://components/spell_button.tscn')

# The game.
onready var spell_select = $ui/spell_select
onready var book_tabs = $ui/spell_select/book_tabs
onready var player_stats = $ui/player_stats
onready var spell_queue = $ui/spell_queue
onready var spell_queue_scroller = $ui/spell_queue/scroll_container
onready var spell_queue_hbox = $ui/spell_queue/scroll_container/hbox

func _ready():
  print('ain nobody bring us')
  g.clear_children(book_grid)
  g.clear_children(spell_queue_hbox)
  for book in g.rules.get_books().values():
    var book_button = book_button_scene.instance()
    book_grid.add_child(book_button)
    book_button.book = book
    book_button.connect('pressed', self, 'on_toggle_book', [book_button])
  print(play_button)
  play_button.connect('pressed', self, 'on_choose_books')
  book_select.visible = true
  spell_select.visible = false
  player_stats.visible = false
  spell_queue.visible = false

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

func on_choose_books():
  var selected_books = get_selected_books()
  if len(selected_books) < 1 or len(selected_books) > 3:
    return
  var selected_book_ids = g.map_method(selected_books, 'get_id')
  g.state = g.GameState.new()
  g.state.init(g.rules, [selected_book_ids, ['conjuration']])
  book_select.visible = false
  spell_select.visible = true
  player_stats.visible = true
  spell_queue.visible = true
  print(g.state.as_json().result)

  g.clear_children(book_tabs)
  
  selected_books = g.state.players[0].books # Not needed but just in case

  # Populate spell list
  for i in range(len(selected_books)):
    var book = selected_books[i]
    var tab = NinePatchRect.new()
    tab.name = book.get_id()
    var spell_list = spell_list_scene.instance()
    spell_list.book = book
    spell_list.handle_spell_clicked(self, 'on_spell_enqueue')
    tab.add_child(spell_list)
    book_tabs.add_child(tab)
  redraw()

func on_spell_enqueue(spell):
  var action = current_action()
  action.append(spell.get_id())
  if not g.state.is_valid_action(0, action):
    return
  var new_button = spell_button_scene.instance()
  new_button.spell = spell
  # TODO: jim: jj
  spell_queue_hbox.add_child(new_button)
  # allow recalculation of scroll size
  yield(get_tree(), 'idle_frame')
  spell_queue_scroller.set_h_scroll(spell_queue_hbox.rect_size.x)
  redraw()

# we funkshuhnuhls now
func enqueued_spells():
  return g.map_member(spell_queue_hbox.get_children(), 'spell')

func current_action():
  return g.map_method(enqueued_spells(), 'get_id')

onready var hp_icon = $ui/player_stats/hp_icon
onready var mp_icon = $ui/player_stats/mp_icon
onready var hp_label = $ui/player_stats/hp_label
onready var mp_label = $ui/player_stats/mp_label

func redraw():
  var me = g.state.players[0]
  var mp_left = me.mp
  for spell in enqueued_spells():
    mp_left -= spell.get_cost()
  hp_label.text = '%d/%d' % [me.hp, me.max_hp]
  mp_label.text = '%d/%d (+%d)' % [mp_left, g.rules.get_mana_cap(), me.mp_regen]
