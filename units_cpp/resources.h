#pragma once
#include <iostream>
#include "json.h"

using json = nlohmann::json;

class Resources {
 public:
  unsigned short coins;
  unsigned short red;
  unsigned short green;
  unsigned short blue;

  Resources();
  Resources(unsigned short coins, unsigned short red, unsigned short green, unsigned short blue);
  void add(const Resources &r);
  void subtract(const Resources &r);
  bool includes(const Resources &r) const;

  Resources operator+(const Resources &r);

  friend std::ostream &operator<<(std::ostream &os, const Resources &r);
};

void to_json(json &j, const Resources &r);
void from_json(const json &j, Resources &r);
bool operator<=(const Resources &r1, const Resources &r2);
bool operator==(const Resources &a, const Resources &b);
