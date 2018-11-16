/* Description of how to create a StickyInstance, from a Sticky and some
 * additional parameters. StickyInvokers are given in Effects that create
 * stickies. */

#pragma once

#include <vector>

#include "duration.h"
#include "sticky.h"
#include "json.h"

using json = nlohmann::json;

class StickyInvoker {
	public:
		StickyInvoker();
		bool operator==(const StickyInvoker &) const;

    std::string get_sticky_id() const;
		int get_amount() const;
		Duration get_duration() const;

		friend void from_json(const json &j, StickyInvoker &sticky_invoker);
		friend void to_json(json &j, const StickyInvoker &sticky_invoker);

	private:
    std::string sticky_id;
		int amount;
		Duration duration;
};

