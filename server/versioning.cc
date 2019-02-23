#include "versioning.h"

#include <iostream>
#include <string>
#include <cstdio>

std::string version_to_string(const Version &v) {
	char buffer[100];
	sprintf(buffer, "%d.%d.%d", v.major, v.minor, v.patch);
	return buffer;
}

Version string_to_version(const std::string s) {
	Version v = { 0, 0, 0 };
	int filled = sscanf(s.c_str(), "%d.%d.%d", &v.major, &v.minor, &v.patch);
	if (filled < 3) {
		return { 0, 0, 0 };
	}
	return v;
}

void from_json(const json& j, Version& v) {
	v.major = j.at(0);
	v.minor = j.at(1);
	v.patch = j.at(2);
}

void to_json(json& j, const Version& v) {
	j = json::array({ v.major, v.minor, v.patch });
}
