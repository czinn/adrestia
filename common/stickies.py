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

	Sticky(id_='mana_drain', name='Mana Drain', kind=SK_id,
		text="Lose 1 mana regeneration per turn.",
		stacks=True,
		trigger=trigger_turn,
		effects=[Effect(EK_mana_regen, ET_special, self=True, amount=-1)],
	),

	Sticky(id_='burning', name='Burning', kind=SK_id,
		text="Deals 2 damage each turn.",
		stacks=True,
		trigger=trigger_turn,
		effects=[Effect(EK_health, ET_attack, self=True, amount=-2)],
	),

	Sticky(id_='exploded', name='Chain Reaction', kind=SK_id,
		text="Deals 3 damage each turn.",
		stacks=True,
		trigger=trigger_turn,
		effects=[Effect(EK_health, ET_attack, self=True, amount=-3)],
	),

	Sticky(id_='storm', name='Storm', kind=SK_delta,
		text="Increase Gathering Storm's damage by 1.",
		stacks=True,
		trigger=trigger_outbound(spell_id=['bloodlust_attack_2']),
		# jim: how would we make this increase damage by 2?
		# charles: Change amount in the sticky invoker
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
		text="Lose 6 health if you learn a spell next turn.",
		trigger=trigger_turn,
		effects=[
			Effect(EK_sticky, ET_special, self=True,
				sticky=StickyInvoker('antitech', duration=duration_turns(2)))
		],
	),

	Sticky(id_='antitech', name='Censure', kind=SK_id,
		text="Lose 6 health if you learn a spell this turn.",
		stacks=True,
		trigger=trigger_spell(effect_type=['tech']),
		effects=[Effect(EK_health, ET_constant, self=True, amount=-6)],
	),

	Sticky(id_='antispell', name='Interdict', kind=SK_id,
		text='Lose 6 health for each spell you cast this turn.',
		stacks=True,
		trigger=trigger_spell(),
		effects=[Effect(EK_health, ET_constant, self=True, amount=-6)],
	),

	Sticky(id_='fury', name='Fury', kind=SK_delta,
		text="Your attacks deal extra damage.",
		trigger=trigger_outbound(effect_type=['attack']),
	),

	Sticky(id_='bloodlust', name='Bloodlust', kind=SK_delta,
		text="Your attacks deal an extra 2 damage damage.",
		stacks=True,
		trigger=trigger_outbound(effect_type=['attack', 'constant']),
	),
]
