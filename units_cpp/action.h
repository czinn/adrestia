/*! Contains the class Action, which represents player actions during a turn.
 *  There can be multiple actions per turn!
 */


#pragma once

#include "colour.h"


enum ActionType { CHOOSE_TECH = 0, BUILD_UNITS };
class Action {
    public:
        Action();
        Action(Colour colour); // Creates a CHOOSE_TECH action, choosing the given Colour.
        Action(std::vector<std::string> units); // Creates a BUILD_UNITS action, building the given units.
        ActionType get_type() const;
        Colour get_colour() const;
        const std::vector<std::string> &get_units() const;

        friend void to_json(json &j, const Action &a);  // Converts the given Action into JSON; result goes into json j
        friend void from_json(const json &j, Action &a);  // Clones an Action from the given JSON into Action a

    private:
        ActionType type;
        Colour colour;
        std::vector<std::string> units;
};


// Actions are equal if they are of the same type, and have chosen the same Colour/units (depending on type)
bool operator==(const Action &a1, const Action &a2);
