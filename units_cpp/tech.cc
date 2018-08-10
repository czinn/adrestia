#include <iostream>
#include <sstream>
#include "tech.h"
#include "json.h"

Tech::Tech() : red(0), green(0), blue(0) {}
Tech::Tech(int red, int green, int blue)
  : red(red), green(green), blue(blue) {
}

void from_json(const json &j, Tech &t) {
  t.red = j.find("red") != j.end() ? j["red"].get<int>() : 0;
  t.green = j.find("green") != j.end() ? j["green"].get<int>() : 0;
  t.blue = j.find("blue") != j.end() ? j["blue"].get<int>() : 0;
}

void to_json(json &j, const Tech &t) {
  // Empty resources is empty object, not null
  j = json({});
  if (t.red) j["red"] = t.red;
  if (t.green) j["green"] = t.green;
  if (t.blue) j["blue"] = t.blue;
}

void Tech::add(const Tech &t) {
  red += t.red;
  green += t.green;
  blue += t.blue;
}

void Tech::max(const Tech &t) {
  red = std::max(red, t.red);
  green = std::max(green, t.green);
  blue = std::max(blue, t.blue);
}

void Tech::increment(Colour colour) {
  switch (colour) {
    case RED: red++; break;
    case GREEN: green++; break;
    case BLUE: blue++; break;
    case BLACK: break;
  }
}

Tech Tech::operator+(const Tech &t) {
  Tech result(*this);
  result.add(t);
  return result;
}

bool Tech::includes(const Tech &t) const {
  return (
      red >= t.red &&
      green >= t.green &&
      blue >= t.blue
  );
}

bool operator<=(const Tech &t1, const Tech &t2) {
  return t2.includes(t1);
}

std::ostream &operator<<(std::ostream &os, const Tech &t) {
  os << json(t);
  return os;
}

bool operator==(const Tech &t1, const Tech &t2) {
  return (
      t1.red == t2.red &&
      t1.green == t2.green &&
      t1.blue == t2.blue
  );
}
