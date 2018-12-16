/* Instance of a sticky attached to a player. Its duration ticks down and its
 * amount or other properties may change. */

#pragma once

#include "duration.h"
#include "json.h"

using json = nlohmann::json;

class Spell;
class Sticky;
class StickyInvoker;
class EffectInstance;
class GameRules;

class StickyInstance {
	public:
		StickyInstance(const Spell &spell, const Sticky &sticky, const StickyInvoker &invoker);
		StickyInstance(const GameRules &rules, const json &j);
		bool operator==(const StickyInstance &) const;

		// Applies the sticky to the effect and possibly generates additional
		// effects. Only effects that trigger the sticky should be passed to this
		// function.
		// player_id is the id of the player to whom the sticky is attached.
		std::vector<EffectInstance> apply_to_effect(size_t player_id, EffectInstance &effect);
		std::vector<EffectInstance> apply_to_effect(
				size_t player_id,
				EffectInstance &effect,
				size_t sticky_index,
				std::vector<json> &events_out);

		// Applies the sticky to the spell and possibly generates additional
		// effects. Only spells that trigger the sticky should be passed to this
		// function.
		// player_id is the id of the player to whom the sticky is attached.
		std::vector<EffectInstance> apply_to_spell(size_t player_id, const Spell &spell);
		std::vector<EffectInstance> apply_to_spell(
				size_t player_id,
				const Spell &spell,
				size_t sticky_index,
				std::vector<json> &events_out);

		// Generates any per-turn effects. This function should only be called if
		// this sticky triggers on turns.
		// player_id is the id of the player to whom the sticky is attached.
		std::vector<EffectInstance> apply_to_turn(size_t player_id);
		std::vector<EffectInstance> apply_to_turn(
				size_t player_id,
				size_t sticky_index,
				std::vector<json> &events_out);

		friend void to_json(json &j, const StickyInstance &sticky);

		// amount is used differently depending on the kind of sticky. For shields,
		// it is the remaining shield strength.
		int amount;
		Duration remaining_duration;
		const Spell &spell;
		const Sticky &sticky;

		template<bool emit_events>
		friend std::vector<EffectInstance> _apply_to_effect(
				StickyInstance &,
				size_t player_id,
				EffectInstance &effect,
				size_t sticky_index,
				std::vector<json> &events_out);

		template<bool emit_events>
		friend std::vector<EffectInstance> _apply_to_spell(
				StickyInstance &,
				size_t player_id,
				const Spell &spell,
				size_t sticky_index,
				std::vector<json> &events_out);

		template<bool emit_events>
		friend std::vector<EffectInstance> _apply_to_turn(
				StickyInstance &,
				size_t player_id,
				size_t sticky_index,
				std::vector<json> &events_out);
};
