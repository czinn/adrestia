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
const GameRules = preload('res://native/game_rules.gdns')

func _init():
	print('test GameRules')
	var rules_file = File.new()
	rules_file.open('res://data/rules.json', File.READ)
	var rules_text = rules_file.get_as_text()
	var rules = GameRules.new()
	rules.load_json_string(rules_text)
	rules_file.close()
	print('test Book')
	var book = rules.get_books()["conjuration"]
	assert(book.get_id() == "conjuration")
	assert(book.get_name() == "Book of Conjuration")
	assert(book.get_spells().size() == 11)
	assert(book.get_spells()[1] == "damage_1")
	print('test Spell')
	var spell = rules.get_spell("shield_2")
	assert(spell.get_id() == "shield_2")
	assert(spell.get_counterspell_selector().is_valid() == false)
	print('test Effect')
	var effect = spell.get_effects()[0]
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
	var sticky = rules.get_sticky("poison")
	assert(sticky.get_id() == "poison")
	assert(sticky.get_effects().size() == 1)
	assert(sticky.triggers_at_end_of_turn())
	quit()
