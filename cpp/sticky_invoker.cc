#include "sticky.h"

#include "effect.h"
#include "spell.h"
#include "effect_instance.h"

//------------------------------------------------------------------------------
// C++ SEMANTICS
//------------------------------------------------------------------------------
StickyInvoker::StickyInvoker() {}

bool StickyInvoker::operator==(const StickyInvoker &other) const {
	return
		(  this->sticky_id == other.sticky_id
		&& this->amount == other.amount
		&& this->duration == other.duration
		);
}

//------------------------------------------------------------------------------
// GETTERS
//------------------------------------------------------------------------------
std::string StickyInvoker::get_sticky_id() const { return sticky_id; }
int StickyInvoker::get_amount() const { return amount; }
Duration StickyInvoker::get_duration() const { return duration; }

//------------------------------------------------------------------------------
// SERIALIZATION
//------------------------------------------------------------------------------
void from_json(const json &j, StickyInvoker &sticky_invoker) {
	sticky_invoker.sticky_id = j.at("sticky_id");
	sticky_invoker.amount = j.find("amount") != j.end() ? j.at("amount").get<int>() : 0;
	sticky_invoker.duration = j.at("duration");
}

void to_json(json &j, const StickyInvoker &sticky_invoker) {
	j["sticky_id"] = sticky_invoker.sticky_id;
	if (sticky_invoker.amount != 0) {
		j["amount"] = sticky_invoker.amount;
	}
	j["duration"] = sticky_invoker.duration;
}
