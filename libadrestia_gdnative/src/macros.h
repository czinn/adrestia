#pragma once
#include <Godot.hpp>
#include <JSONParseResult.hpp>
#include <JSON.hpp>

template<typename T>
godot::Ref<godot::JSONParseResult> to_godot_json(T &t) {
  nlohmann::json j = t;
  godot::String str = j.dump().c_str();
  return godot::JSON::parse(str);
}
