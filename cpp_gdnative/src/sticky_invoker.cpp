#include "sticky_invoker.h"

#define CLASSNAME StickyInvoker

using namespace godot;

namespace godot {
	const char *StickyInvoker::resource_path = "res://native/sticky_invoker.gdns";

	StickyInvoker::StickyInvoker() {
		Duration_ = ResourceLoader::load(Duration::resource_path);
	}

	void StickyInvoker::_register_methods() {
		REGISTER_METHOD(get_sticky_id);
		REGISTER_METHOD(get_amount);
		REGISTER_METHOD(get_duration);
		REGISTER_NULLABLE
		REGISTER_JSONABLE
	}

	FORWARD_STRING_GETTER(get_sticky_id)
	FORWARD_GETTER(int, get_amount)

	Variant StickyInvoker::get_duration() const {
		auto [v, thing] = instance<Duration>(Duration_);
		thing->set_ptr(new ::Duration(_ptr->get_duration()));
		return v;
	}

	IMPL_NULLABLE
	IMPL_JSONABLE
}
