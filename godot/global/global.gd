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
