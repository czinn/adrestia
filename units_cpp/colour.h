/* The Colour class. Really just an enum. */


#pragma once

#include "json.h"

using nlohmann::json;


enum Colour { BLACK, RED, GREEN, BLUE };

void to_json(json& j, const Colour& colour);
void from_json(const json& j, Colour& colour);
