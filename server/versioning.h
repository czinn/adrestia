#pragma once

#include <string>
#include "../cpp/json.h"

using json = nlohmann::json;

struct Version {
	int major = 0;
	int minor = 0;
	int patch = 0;
	friend void from_json(const json &j, Version &rules);
	friend void to_json(json &j, const Version &rules);
};

std::string version_to_string(const Version &v);
Version string_to_version(const std::string s);

inline bool operator< (const Version& lhs, const Version& rhs) {
	return lhs.major < rhs.major || (lhs.major == rhs.major && 
			(lhs.minor < rhs.minor || (lhs.minor == rhs.minor &&
				lhs.patch < rhs.patch)));
}
inline bool operator> (const Version& lhs, const Version& rhs) { return rhs < lhs; }
inline bool operator<=(const Version& lhs, const Version& rhs) { return !(lhs > rhs); }
inline bool operator>=(const Version& lhs, const Version& rhs) { return !(lhs < rhs); }
inline bool operator==(const Version& lhs, const Version& rhs) {
	return lhs.major == rhs.major && lhs.minor == rhs.minor && lhs.patch == rhs.patch;
}
inline bool operator!=(const Version& lhs, const Version& rhs) { return !(lhs == rhs); }
