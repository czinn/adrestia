// Makes C++ suck less.

#include <vector>

//------------------------------------------------------------------------------
// HIGHER KINDED TYPES
//------------------------------------------------------------------------------
template<template<class> class C, typename T>
bool contains(const C<T> &container, const T &needle) {
	return std::find(container.begin(), container.end(), needle) != container.end();
}

template<template<class, class> class C, typename T, typename V>
bool contains(const C<T, V> &container, const T &needle) {
	return std::find(container.begin(), container.end(), needle) != container.end();
}
