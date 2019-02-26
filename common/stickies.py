from rules_schema import *

stickies_list: List[Sticky] = [
  Sticky(id_='shield', name='Shield', kind=SK_shield,
    text="Blocks damage from attacks.",
    trigger=trigger_inbound(effect_type=['attack']),
  ),

  Sticky(id_='turret', name='Turret', kind=SK_id,
    text="Deals 3 damage each turn.",
    stacks=True,
    trigger=trigger_turn,
    effects=[Effect(EK_health, ET_attack, self=False, amount=-3)],
  ),

  Sticky(id_='wounded', name='Wounded', kind=SK_id,
    text="Lose 3 health each turn.",
    stacks=True,
    trigger=trigger_turn,
    effects=[Effect(EK_health, ET_constant, self=True, amount=-3)],
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

  Sticky(id_='antitech_next', name='Censure', kind=SK_id,
    text="Lose 15 health if you learn a spell next turn.",
    trigger=trigger_turn,
    effects=[
      Effect(EK_sticky, ET_special, self=True,
        sticky=StickyInvoker('antitech', duration=duration_turns(2)))
    ],
  ),

  Sticky(id_='antitech', name='Censure', kind=SK_id,
    text="Lose 15 health if you learn a spell this turn.",
    stacks=True,
    trigger=trigger_spell(effect_type=['tech']),
    effects=[Effect(EK_health, ET_constant, self=True, amount=-15)],
  ),

  Sticky(id_='bloodlust', name='Bloodlust', kind=SK_delta,
    text="Your attacks deal an extra damage.",
    stacks=True,
    trigger=trigger_outbound(effect_type=['attack', 'constant']),
  ),
]
