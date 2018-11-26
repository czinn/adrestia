extends Node

const Book = preload('res://native/book.gdns')
const Duration = preload('res://native/duration.gdns')
const DurationUnit = preload('res://native/duration_unit.gd')
const StickyInvoker = preload('res://native/sticky_invoker.gdns')
const Effect = preload('res://native/effect.gdns')
const Spell = preload('res://native/spell.gdns')
const EffectType = preload('res://native/effect_type.gd')
const Sticky = preload('res://native/sticky.gdns')
const GameRules = preload('res://native/game_rules.gdns')
const GameState = preload('res://native/game_state.gdns')
const GameView = preload('res://native/game_view.gdns')
const Strategy = preload('res://native/strategy.gdns')

onready var tooltip_scene = preload('res://components/tooltip.tscn')

onready var scene_loader = get_node('/root/scene_loader')
var rules
var state
var ai

func _ready():
	var rules_file = File.new()
	rules_file.open('res://data/rules.json', File.READ)
	rules = GameRules.new()
	rules.load_json_string(rules_file.get_as_text())
	rules_file.close()

static func clear_children(node):
	for i in range(0, node.get_child_count()):
		node.get_child(i).queue_free()

static func map_method(list, method):
	var result = []
	for elem in list:
		result.append(elem.call(method))
	return result

static func map_member(list, member):
	var result = []
	for elem in list:
		result.append(elem.get(member))
	return result

func summon_tooltip(target, text):
	var tooltip = tooltip_scene.instance()
	tooltip.text = text
	var pos = target.get_global_rect().position
	var above = pos.y > 980 - pos.y + target.rect_size.y
	var y = pos.y if above else (pos.y + target.rect_size.y)
	tooltip.set_target(pos.x + target.rect_size.x / 2, y, above)
	get_node("/root/root/ui").add_child(tooltip)

func event_is_pressed(event):
	return event is InputEventMouseButton \
		and event.button_index == BUTTON_LEFT \
		and event.pressed
