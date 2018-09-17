/* Contains the class Tech, which stores a collection of techs. */

#pragma once

#include <iostream>

#include "colour.h"
#include "json.h"

using json = nlohmann::json;


class Tech {
	public:
		int red;
		int green;
		int blue;

		Tech();
		Tech(int red, int green, int blue);
		void add(const Tech &t);
		void max(const Tech &t);  // Sets all values of this tech to the max of (this tech, the given tech)
		void increment(Colour colour);  // Increment this colour by one. Will not increment BLACK.
		bool includes(const Tech &t) const;  // Check if this tech is a proper superset of the given one.

		Tech operator+(const Tech &t);

		friend std::ostream &operator<<(std::ostream &os, const Tech &t);
};

void to_json(json &j, const Tech &t);
void from_json(const json &j, Tech &t);
bool operator<=(const Tech &t1, const Tech &t2);
bool operator==(const Tech &t1, const Tech &t2);
