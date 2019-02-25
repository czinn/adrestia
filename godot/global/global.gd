extends Node

const Book = preload('res://native/book.gdns')
const Duration = preload('res://native/duration.gdns')
const DurationUnit = preload('res://native/duration_unit.gd')
const StickyInvoker = preload('res://native/sticky_invoker.gdns')
const Effect = preload('res://native/effect.gdns')
const EffectKind = preload('res://native/effect_kind.gd')
const Spell = preload('res://native/spell.gdns')
const EffectType = preload('res://native/effect_type.gd')
const Sticky = preload('res://native/sticky.gdns')
const GameRules = preload('res://native/game_rules.gdns')
const GameState = preload('res://native/game_state.gdns')
const GameView = preload('res://native/game_view.gdns')
const Strategy = preload('res://native/strategy.gdns')

const Tweener = preload('res://global/tweener.gd')

signal tooltip_closed()

onready var tooltip_scene = preload('res://components/tooltip.tscn')
onready var spell_button_scene = preload('res://components/spell_button.tscn')
onready var delta_anim_scene = preload('res://components/delta_anim.tscn')
onready var confirm_popup_scene = preload('res://components/confirm_popup.tscn')
onready var text_entry_popup_scene = preload('res://components/text_entry_popup.tscn')

onready var scene_loader = get_node('/root/scene_loader')
onready var network = get_node('/root/networking')
onready var drag_drop = get_node('/root/drag_drop')

# Read from rules.json
var app_version = null # e.g. [1, 0, 0]

var loaded = false
var backend = null
var tutorial_overlay = null
var tooltip = null # Currently displayed tooltip

# jim: So, why the hell is this an array? Because we have to keep all versions
# of the rules we've ever conceivably used alive, and it's hard to keep track
# of exactly when we can be sure that rules can be freed, so we just never free
# them. The .back() of this array will be the freshest rules, and backends can
# override these rules (as retrieved by [get_rules]). But the rules must always
# live.
var rules = []

func _ready():
	pass

func get_rules():
	if backend == null:
		return rules.back()
	return backend.rules

func get_default_rules():
	return rules.back()

func update_rules(json_string):
	var new_rules = GameRules.new()
	new_rules.load_json_string(json_string)
	rules.append(new_rules)
	save()

static func sum(list):
	var result = 0
	for elem in list:
		result += elem
	return result

static func clear_children(node):
	for i in range(0, node.get_child_count()):
		node.get_child(i).queue_free()

# Gets child by name rather than index.
static func child(parent, child_name):
	# Not recursive; not owned.
	return parent.find_node(child_name, false, false)

static func map(list, f):
	var result = []
	for elem in list:
		result.append(f(elem))
	return result

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

static func dispose(node):
	node.queue_free()

static func load_or(path, path_default):
	# jim: This spits errors into the debugger but is necessary to have custom
	# art show in Android. https://github.com/godotengine/godot/issues/8773
	# jim: Actually, this is a good thing since it lets us know exactly which
	# things still need assets and annoys us until we create them.
	var thing = load(path)
	if thing == null:
		thing = load(path_default)
	return thing

static func get_thing_texture(thing_type, thing_id):
	return load_or(
		'res://art-built/%s/%s.png' % [thing_type, thing_id],
		'res://art-built/%s/placeholder.png' % thing_type)

static func get_book_texture(book_id):
	return get_thing_texture("book", book_id)

static func get_spell_texture(spell_id):
	return get_thing_texture("spells", spell_id)

static func get_sticky_texture(sticky_id):
	return get_thing_texture("stickies", sticky_id)

func make_spell_buttons(spells, show_stats = false, display_filter = null, enabled_filter = null, unlocked_filter = null):
	var result = []
	for spell_id in spells:
		var spell = get_rules().get_spell(spell_id)
		if display_filter != null and not display_filter.call_func(spell):
			continue
		var spell_button = spell_button_scene.instance()
		spell_button.show_stats = show_stats
		spell_button.enabled = enabled_filter == null or enabled_filter.call_func(spell)
		spell_button.show_unlock = \
				unlocked_filter == null or not unlocked_filter.call_func(spell)
		# Set the spell last so that we don't redraw so many times
		spell_button.spell = spell
		result.append(spell_button)
	return result

func close_tooltip():
	if tooltip != null:
		tooltip.get_parent().remove_child(tooltip)
		tooltip = null
		emit_signal('tooltip_closed')

func summon_tooltip(target, text):
	close_tooltip()
	tooltip = tooltip_scene.instance()
	tooltip.text = text
	var pos = target.get_global_rect().position
	var above = pos.y > 20
	var y = pos.y if above else (pos.y + target.rect_size.y)
	tooltip.set_target(pos.x + target.rect_size.x / 2, y, above)
	get_node("/root").add_child(tooltip)

func summon_spell_tooltip(target, spell):
	summon_tooltip(target, "[b]%s[/b]\n%s" % [spell.get_name(), spell.get_text()])

func summon_sticky_tooltip(target, sticky):
	summon_tooltip(target, "[b]%s[/b]\n%s" % [sticky.get_name(), sticky.get_text()])

func summon_delta(target, value, color):
	var delta = delta_anim_scene.instance()
	var pos = target.get_global_rect().position
	var fadeup = pos.y >= 80
	delta.margin_top = pos.y - 80 if fadeup else (pos.y + target.rect_size.y)
	delta.margin_left = pos.x + (target.rect_size.x / 2) - 25
	get_node("/root").add_child(delta)
	delta.play_text_and_color(("+" if value > 0 else "") + str(value), color, fadeup)

func summon_confirm(text):
	var confirm = confirm_popup_scene.instance()
	confirm.text = text
	get_node("/root").add_child(confirm)
	return confirm

func summon_notification(text):
	scene_loader.notification.show_notification(text)

func summon_text_entry(text, default_text):
	var popup = text_entry_popup_scene.instance()
	popup.text = text
	popup.default_text = default_text
	get_node("/root").add_child(popup)
	return popup

func event_is_pressed(event):
	return event is InputEventMouseButton \
		and event.button_index == BUTTON_LEFT \
		and event.pressed

func tween(thing, to_pos, time):
	var tw = Tweener.new(get_node('/root'))
	tw.tween_to(thing, to_pos, time)
	return tw

func safe_disconnect(object, signal_, target, method):
	if object.is_connected(signal_, target, method):
		object.disconnect(signal_, target, method)

func remove_tutorial_overlay():
	if tutorial_overlay != null && tutorial_overlay.get_parent() != null:
		tutorial_overlay.get_parent().remove_child(tutorial_overlay)
	tutorial_overlay = null

func remove_backend():
	if backend != null:
		backend.free()
		backend = null

func compare_versions(a, b):
	for i in range(len(a)):
		if a[i] < b[i]:
			return -1
		elif a[i] > b[i]:
			return 1
	return 0

func string_to_version(s):
	var p = s.split('.')
	return [int(p[0]), int(p[1]), int(p[2])]

func version_to_string(v):
	return '%d.%d.%d' % [v[0], v[1], v[2]]

# Data to persist between sessions.
const save_path = 'user://saved_data.json'
const default_rules_path = 'res://data/rules.json'
var auth_uuid
var auth_pwd
var first_play
var user_name
var tag
var multiplayer_wins
# var rules # (declared above)

func save():
	var data = {
		'auth_uuid': auth_uuid,
		'auth_pwd': auth_pwd,
		'first_play': first_play,
		'user_name': user_name,
		'tag': tag,
		'rules': rules.back().as_json().result,
		'multiplayer_wins': multiplayer_wins,
	}
	var file = File.new()
	file.open(save_path, File.WRITE)
	file.store_line(to_json(data))
	file.close()

func dict_has(dict, key, default):
	if dict.has(key):
		return dict[key]
	else:
		return default

func load():
	var file = File.new()
	var data
	if file.file_exists(save_path):
		file.open(save_path, File.READ)
		data = parse_json(file.get_line())
	else:
		print('No save data.')
		data = {}

	# Load rules from file by default.
	var rules_file = File.new()
	rules = [GameRules.new()]
	rules_file.open(default_rules_path, File.READ)
	rules[0].load_json_string(rules_file.get_as_text())
	rules_file.close()
	app_version = rules.back().get_version()

	# Default values for persisted data
	auth_uuid = dict_has(data, 'auth_uuid', null)
	auth_pwd = dict_has(data, 'auth_pwd', null)
	first_play = dict_has(data, 'first_play', true)
	user_name = dict_has(data, 'user_name', null)
	tag = dict_has(data, 'tag', null)
	multiplayer_wins = dict_has(data, 'multiplayer_wins', null)

	# Override rules with those from saved_data if they're newer.
	var rules_json = dict_has(data, 'rules', null)
	if rules_json != null:
		var rules_saved = GameRules.new()
		rules_saved.load_json_string(JSON.print(rules_json))
		if compare_versions(rules.back().get_version(), rules_saved.get_version()) < 0:
			rules.append(rules_saved)

	if file.is_open():
		file.close()
