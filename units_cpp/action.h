#pragma once

#include "colour.h"


enum ActionType { CHOOSE_TECH = 0, BUILD_UNITS };
class Action {
    public:
        Action();
        Action(Colour colour); // Creates a CHOOSE_TECH action
        Action(std::vector<std::string> units); // Creates a BUILD_UNITS action
        ActionType get_type() const;
        Colour get_colour() const;
        const std::vector<std::string> &get_units() const;

        friend void to_json(json &j, const Action &a);
        friend void from_json(const json &j, Action &a);
    private:
        ActionType type;
        Colour colour;
        std::vector<std::string> units;
};


// Actions are equal if they are of the same type, and have chosen the same Colour/units (DEPENDING ON TYPE)
bool operator==(const Action &a1, const Action &a2);
