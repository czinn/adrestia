#include "colour.h"

void to_json(json& j, const Colour& colour) {
	switch (colour) {
		case BLACK: j = "BLACK"; break;
		case RED  : j = "RED"  ; break;
		case GREEN: j = "GREEN"; break;
		case BLUE : j = "BLUE" ; break;
	}
}

void from_json(const json& j, Colour& colour) {
	if (j == "BLACK") {
		colour = BLACK;
	} else if (j == "RED") {
		colour = RED;
	} else if (j == "GREEN") {
		colour = GREEN;
	} else if (j == "BLUE") {
		colour = BLUE;
	}
}
