/* Contains the class UnitKind, which defines a unit in general (an
 * uninstantiated unit) */

#pragma once

#include <vector>

#include "colour.h"
#include "tech.h"
#include "json.h"

using json = nlohmann::json;


class UnitKind {
	/* A class representing a kind of unit, rather than a particular instance. */

	public:
		UnitKind();
		~UnitKind();
		std::string get_id() const;
		std::string get_name() const;
		Colour get_colour() const;
		int get_health() const;
		int get_shields() const;
		int get_width() const;
		int get_build_time() const;
		const std::vector<int> get_attack() const;
		int get_cost() const;
		const std::shared_ptr<Tech> get_tech() const;
		int get_font() const;
		std::string get_image() const;
		const std::vector<int> get_tiles() const;
		char get_label() const;

		friend void to_json(json &j, const UnitKind &kind);
		friend void from_json(const json &j, UnitKind &kind);

	private:
		std::string id;
		std::string name;
		Colour colour;
		int health;
		int shields;
		int width;
		int build_time;
		std::vector<int> attack;
		int cost;
		int font;
		std::shared_ptr<Tech> tech; // nullptr for no tech (not buildable)
		std::string image;
		std::vector<int> tiles;
		char label;
};

