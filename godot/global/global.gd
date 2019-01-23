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

onready var scene_loader = get_node('/root/scene_loader')
onready var network = get_node('/root/networking')
onready var drag_drop = get_node('/root/drag_drop')
var loaded = false
var backend = null
var tooltip = null # Currently displayed tooltip
var rules = null setget ,get_rules

func _ready():
	pass

func get_rules():
	if backend == null:
		return null
	return backend.rules

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
