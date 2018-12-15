from dataclasses import dataclass, field
from typing import List, Dict, Optional, Any, Union, Callable

# Duration
# TODO: jim: is this too dank?
Duration = dict
duration_steps = lambda x: {'steps': x}
duration_turns = lambda x: {'turns': x}
duration_game = {'game': True}

@dataclass
class Selector:
    book_id: Optional[List[str]] = None
    spell_id: Optional[List[str]] = None
    effect_type: Optional[List[str]] = None

@dataclass
class Trigger:
    inbound: Optional[bool] = None
    spell: Optional[Selector] = None
    effect: Optional[Selector] = None
    turn: Optional[bool] = None

def trigger_spell(**kwargs): return Trigger(spell=Selector(**kwargs))
def trigger_effect(inbound, **kwargs): return Trigger(effect=Selector(**kwargs), inbound=inbound)
def trigger_inbound(**kwargs): return trigger_effect(True, **kwargs)
def trigger_outbound(**kwargs): return trigger_effect(False, **kwargs)
trigger_turn = Trigger(turn=True)

@dataclass
class StickyInvoker:
    sticky_id: str
    duration: Duration
    amount: Optional[int] = None

@dataclass
class Sticky:
    id_: str
    name: str
    text: str
    kind: str
    trigger: Trigger
    effects: Optional[List['Effect']] = None
    stacks: Optional[bool] = None

# StickyKind
SK_delta = 'delta'
SK_shield = 'shield'
SK_damper = 'damper'
SK_super_shield = 'super_shield'
SK_id = 'id'

# EffectKind
EK_health = 'health'
EK_tech = 'tech'
EK_mana = 'mana'
EK_mana_regen = 'mana_regen'
EK_sticky = 'sticky'

# EffectType
ET_attack = 'attack'
ET_shield = 'shield'
ET_special = 'special'
ET_poison = 'poison'
ET_constant = 'constant'
ET_counter = 'counter'
ET_tech = 'tech'
ET_heal = 'heal'

@dataclass
class Effect:
    kind: str
    effect_type: str
    self: bool = False
    amount: Optional[int] = None
    sticky: Optional[StickyInvoker] = None

@dataclass
class Spell:
    sub_id: str
    name: str
    text: str
    tech: int
    level: int
    cost: int
    effects: List[Effect] = field(default_factory=list)
    counterspell: Optional[Selector] = None
    id_: Optional[str] = None # will be set by the containing Book
    book: Optional[str] = None # will be set by the containing Book

@dataclass
class Book:
    id_: str
    name: str
    spells: List[Spell]

# Convenience functions.

# Converts arbitrary Python type to JSONable basic types.
# Renames id_ fields to id. Removes sub_id fields.
def deconstruct(thing: Any, special_handlers: Dict[Any, Callable[[Any], Any]]={}) -> Any:
    if type(thing) in special_handlers:
        return special_handlers[type(thing)](thing)
    elif type(thing) == dict:
        return {k:deconstruct(v, special_handlers) for k,v in thing.items()}
    elif type(thing) == list:
        return [deconstruct(x, special_handlers) for x in thing]
    elif hasattr(thing, '__dict__'):
        d = {k:deconstruct(v, special_handlers) for k,v in thing.__dict__.items() if v != None}
        if 'id_' in d:
            d['id'] = d['id_']
            del d['id_']
        if 'sub_id' in d:
            del d['sub_id']
        return d
    else:
        return thing

# WE DSL NOW
effect_mana_regen = lambda amt: Effect(EK_mana_regen, ET_special, self=True, amount=1)
effect_attack = lambda dmg: Effect(EK_health, ET_attack, self=False, amount=-dmg)
effect_shield = lambda amt, duration=duration_turns(1): Effect(EK_sticky, ET_shield, self=True,
        sticky=StickyInvoker('shield', amount=amt, duration=duration))
spell_tech = lambda **kwargs: Spell('tech', tech=0, level=0, cost=0,
        effects=[Effect(EK_tech, ET_tech, self=True, amount=1)], **kwargs)
