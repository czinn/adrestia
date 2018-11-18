extends SceneTree

# Tests the integration of gdnative modules.
# Also serves as an example of how exactly to use them.

const Book = preload('res://native/book.gdns')
# const Duration = preload('res://native/duration.gdns')
const DurationUnit = preload('res://native/duration_unit.gd')
# const StickyInvoker = preload('res://native/sticky_invoker.gdns')
# const Effect = preload('res://native/effect.gdns')
const Spell = preload('res://native/spell.gdns')
const EffectType = preload('res://native/effect_type.gd')
const Sticky = preload('res://native/sticky.gdns')

func _init():
	print('test Book')
	var b = Book.new()
	b.load_json_string('{"id": "conjuration", "name": "Book of Conjuration", "spells": ["yes", "no", "maybe_so"]}')
	assert(b.get_id() == "conjuration")
	assert(b.get_name() == "Book of Conjuration")
	assert(b.get_spells().size() == 3)
	assert(b.get_spells()[1] == "no")
	print('test Spell')
	var spell = Spell.new()
	spell.load_json_string(' { "id": "shield_3", "name": "Greater Shield", "book": "conjuration", "tech": 3, "level": 3, "cost": 5, "text": "Block 8 damage.", "effects": [ { "kind": "sticky", "self": true, "effect_type": "shield", "sticky": { "sticky_id": "shield", "amount": 8, "duration": { "turns": 1 } } } ] }')
	assert(spell.get_id() == "shield_3")
	print('test Effect')
	var effect = spell.get_effects()[0]
	effect.load_json_string('{"kind": "sticky", "self": true, "effect_type": "shield", "sticky": { "sticky_id": "shield", "amount": 4, "duration": { "turns": 2 } } }')
	assert(effect.get_effect_type() == EffectType.ET_SHIELD)
	print('test StickyInvoker')
	var sticky_invoker = effect.get_sticky_invoker()
	assert(sticky_invoker.get_sticky_id() == "shield")
	print('test Duration')
	var duration = sticky_invoker.get_duration()
	assert(duration.get_value() == 2)
	assert(duration.get_unit() == DurationUnit.TURNS)
	# TODO: charles: We shouldn't really be able to modify this, but I guess type
	# safety is something we lose when we do all this wrapping. Fix if possible.
	duration.subtract_step()
	duration.subtract_turn()
	assert(duration.get_value() == 1)
	print('test Sticky')
	var sticky = Sticky.new()
	sticky.load_json_string('{ "id": "poison", "name": "Poison", "text": "Deals 1 damage each turn.", "kind": "id", "trigger": { "turn": true }, "effects": [ { "kind": "health", "self": true, "effect_type": "poison", "amount": -1 } ] }')
	assert(sticky.get_id() == "poison")
	assert(sticky.get_effects().size() == 1)
	assert(sticky.triggers_at_end_of_turn())
	quit()
