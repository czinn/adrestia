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

func summon_tooltip(scene, target, text, above=true):
	var tooltip = tooltip_scene.instance()
	tooltip.text = text
	var y = target.rect_position.y if above else (target.rect_position.y + target.rect_size.y)
	tooltip.set_target(target.rect_position.x + target.rect_size.x / 2, y, above)
	scene.add_child(tooltip)
	print(y)
	print(tooltip.margin_top)
	print(tooltip.margin_bottom)
