#include "action.h"
#include "colour.h"


//----------------------------------------------------------------------------------------------------------------------
// CONSTRUCTORS
//----------------------------------------------------------------------------------------------------------------------
Action::Action() {
}


Action::Action(Colour colour) : type(CHOOSE_TECH), colour(colour) {
    /*! \brief The CHOOSE_TECH constructor.
     *  \param colour The chosen tech.
     */
}


Action::Action(std::vector<std::string> units) : type(BUILD_UNITS), units(units) {
    /*! \brief The BUILD_UNITS constructor.
     *  \param units A list of unit ids.
     */
}


//----------------------------------------------------------------------------------------------------------------------
// GETTERS AND SETTERS
//----------------------------------------------------------------------------------------------------------------------
ActionType Action::get_type() const {
    return type;
}


Colour Action::get_colour() const {
    /*! \brief Gets the colour of this action. Behaviour is undefined if this is not a CHOOSE_TECH action! */
    return colour;
}


const std::vector<std::string> &Action::get_units() const {
    /*! \brief Returns the unit ids built as part of this BUILD_UNITS action. Behaviour is undefined if this is not a
     *         BUILD_UNITS action!
     */

    return units;
}


//----------------------------------------------------------------------------------------------------------------------
// OTHER METHODS
//----------------------------------------------------------------------------------------------------------------------
void to_json(json &j, const Action &a) {
    if (a.type == CHOOSE_TECH) {
        j["type"] = "CHOOSE_TECH";
        j["colour"] = a.colour;
    }
    else if (a.type == BUILD_UNITS) {
        j["type"] = "BUILD_UNITS";
        j["units"] = a.get_units();
    }
}


void from_json(const json &j, Action &a) {
    if (j["type"] == "CHOOSE_TECH") {
        a.type = CHOOSE_TECH;
        a.colour = j["colour"];
    }
    else if (j["type"] == "BUILD_UNITS") {
        a.type = BUILD_UNITS;
        for (auto it = j["units"].begin(), end = j["units"].end(); it != end; it++) {
            a.units.push_back(*it);
        }
    }
}


bool operator==(const Action &a1, const Action &a2) {
    /*! \brief Checks whether both Actions represent the same "action."
     *         CHOOSE_TECH actions are equal if the colour they chose is the same.
     *         BUILD_UNITS actions are equal if the units they built are the same.
     */

    if (a1.get_type() != a2.get_type()) {
        return false;
    }

    if (a1.get_type() == CHOOSE_TECH) {
        // CHOOSE_TECH Actions are equal if the colour they chose is the same.
        return a1.get_colour() == a2.get_colour();
    }
    else if (a1.get_type() == BUILD_UNITS) {
        // BUILD_UNITS Actions are equal if the units they built are equal
        if (a1.get_units().size() != a2.get_units().size()) {
            return false;
        }

        for (int i = 0; i < a1.get_units().size(); i++) {
            if (a1.get_units()[i] != a2.get_units()[i]) {
                return false;
            }
        }

        return true;
    }

    return false;
}
