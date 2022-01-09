#pragma once

#define USE_DOUBLE
#include "hsm.hpp"
using namespace hsm;
#include <vector>

typedef Vector3<Float> Color;

inline Color operator * (const Color& c1, const Color& c2) {
	return Color(c1.x * c2.x, c1.y * c2.y, c1.z * c2.z);
}

const int samplesPerPixel = 5;

std::ostream& operator << (std::ostream &o, const Color c) {
	auto r = c.x;
	auto g = c.y;
	auto b = c.z;

	auto scale = 1.0f / samplesPerPixel;
	r = sqrt(r * scale);
	g = sqrt(g * scale);
	b = sqrt(b * scale);

	o << static_cast<int>(256.0f * Clamp<Float>(r,0.0f,0.999f)) << ' '
	  << static_cast<int>(256.0f * Clamp<Float>(g,0.0f,0.999f)) << ' '
	  << static_cast<int>(256.0f * Clamp<Float>(b,0.0f,0.999f)) << std::endl;
	return o;
}