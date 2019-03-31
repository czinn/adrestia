extends HBoxContainer

const Resources = preload("res://lib/resources.gd")
const ChooseResources = preload("res://lib/actions/choose_resources.gd")

var resource_total = 7

onready var reset_button = get_node("../ResetButton")
onready var begin_button = get_node("../BeginButton")
onready var remaining_label = get_node("../RemainingLabel")
onready var g = get_node("/root/global")

onready var resources = Resources.empty()

onready var counters = [$RCounter, $GCounter, $BCounter]
var color_chars = "rgb"

func _ready():
  get_tree().set_auto_accept_quit(false)
  get_tree().set_quit_on_go_back(false)
  for i in range(counters.size()):
    counters[i].connect("increment", self, "_on_pressed", [color_chars[i], 1])
    counters[i].connect("decrement", self, "_on_pressed", [color_chars[i], -1])
    counters[i].set_total(resource_total)
  counters[0].set_color(Color(0.77, 0, 0))
  counters[1].set_color(Color(0.19, 0.42, 0.11))
  counters[2].set_color(Color(0.07, 0.29, 0.77))
  reset_button.connect("button_down", self, "_on_reset_pressed")
  begin_button.connect("button_down", self, "_on_begin_pressed")
  update_labels()

func update_labels():
  for i in range(counters.size()):
    counters[i].set_value(resources.get(color_chars[i]))
  remaining_label.set_text("Remaining resources: %d" % (resource_total - resources.total()))
  begin_button.disabled = resources.total() != resource_total
  
func _on_pressed(color_char, delta):
  if resources.total() + delta <= resource_total && resources.get(color_char) + delta >= 0:
    resources.set(color_char, resources.get(color_char) + delta)
    update_labels()

func _on_reset_pressed():
  resources.r = 0
  resources.g = 0
  resources.b = 0
  update_labels()

func _on_begin_pressed():
  if resources.total() == resource_total:
    g.man.perform_action(ChooseResources.new(0, resources))
    begin_button.disabled = true
    g.man.start_game(self, '_on_game_start')

func _on_game_start():
  get_tree().change_scene("res://scenes/game.tscn")

func _notification(what):
  if what == MainLoop.NOTIFICATION_WM_GO_BACK_REQUEST or what == MainLoop.NOTIFICATION_WM_QUIT_REQUEST:
    get_tree().change_scene("res://scenes/title.tscn")
