from rules_schema import *

stickies_list: List[Sticky] = [
	Sticky(id_='shield', name='Shield', kind=SK_shield,
		text="Blocks damage from attacks.",
		trigger=trigger_inbound(effect_type=['attack']),
	),

	Sticky(id_='poison', name='Poison', kind=SK_id,
		text="Deals 1 damage each turn.",
		stacks=True,
		trigger=trigger_turn,
		effects=[Effect(EK_health, ET_poison, self=True, amount=-1)],
	),

	Sticky(id_='storm', name='Storm', kind=SK_delta,
		text="Increase Gathering Storm's damage by 1.",
		trigger=trigger_outbound(spell_id=['chain_1']),
		# TODO: jim: how would we make this increase damage by 2?
	),

	Sticky(id_='spell_poison', name='Fatigue', kind=SK_id,
		text="Casting spells deals 1 poison damage.",
		trigger=trigger_spell(),
		effects=[Effect(EK_health, ET_poison, self=True, amount=-1)],
	),

	Sticky(id_='damper', name='Damper', kind=SK_damper,
		text="Decreases the strength of incoming attacks.",
		trigger=trigger_inbound(effect_type=['attack']),
	),

	Sticky(id_='poison_shield', name='Antidote', kind=SK_shield,
		text="Blocks damage from poison.",
		trigger=trigger_inbound(effect_type=['poison']),
	),

	Sticky(id_='shields_from_spells', name='Fortress', kind=SK_id,
		text="When you cast a spell, gain a shield that blocks 2 damage.",
		trigger=trigger_spell(),
		effects=[
			Effect(EK_sticky, ET_shield, self=True, sticky=StickyInvoker('shield', duration=duration_turns(2), amount=2)),
		],
	),

	Sticky(id_='super_shield', name='Crystal Shield', kind=SK_super_shield,
		text="Blocks all damage from the next attack.",
		trigger=trigger_inbound(effect_type=['attack']),
	),

	Sticky(id_='suck_mana', name='Suck Mana', kind=SK_id,
		text="Enemy is draining 1 mana per turn.",
		stacks=True,
		trigger=trigger_turn,
		effects=[
			Effect(EK_mana, ET_special, self=True, amount=-1),
			Effect(EK_mana, ET_special, amount=1),
		],
	),

	Sticky(id_='antitech_next', name='Censure', kind=SK_id,
		text="Lose 5 health if you cast a tech spell next turn.",
		trigger=trigger_turn,
		effects=[
			Effect(EK_sticky, ET_special, self=True,
				sticky=StickyInvoker('antitech', duration=duration_turns(1)))
		],
	),

	Sticky(id_='antitech', name='Censure', kind=SK_id,
		text="Lose 5 health if you cast a tech spell.",
		trigger=trigger_spell(effect_type=['tech']),
		effects=[Effect(EK_health, ET_constant, self=True, amount=-5)],
	),

	Sticky(id_='antispell', name='Interdict', kind=SK_id,
		text='Lose 6 health for each spell you cast this turn.',
		trigger=trigger_spell(),
		effects=[Effect(EK_health, ET_constant, self=True, amount=-6)],
	),

	Sticky(id_='bloodlust', name='Bloodlust', kind=SK_id,
		text='All spells you cast deal an additional 2 damage.',
		trigger=trigger_spell(),
		effects=[Effect(EK_health, ET_attack, self=True, amount=-2)],
	),

	Sticky(id_='fury', name='Fury', kind=SK_delta,
		text="Your attacks deal 3 extra damage.",
		trigger=trigger_inbound(effect_type=['attack']),
	),
]
