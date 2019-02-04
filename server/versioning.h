#pragma once

#include <string>

struct Version {
	int major;
	int minor;
	int patch;
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
