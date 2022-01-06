#pragma once

#define USE_DOUBLE
#include "hsm.hpp"
using namespace hsm;

typedef Vector3<Float> Color;

const int samplesPerPixel = 50;

std::ostream& operator << (std::ostream &o, const Color c) {
	auto r = c.x;
	auto g = c.y;
	auto b = c.z;

	auto scale = 1.0f / samplesPerPixel;
	r *= scale;
	g *= scale;
	b *= scale;

	o << static_cast<int>(256.0f * Clamp<Float>(r,0.0f,0.999f)) << ' '
	  << static_cast<int>(256.0f * Clamp<Float>(g,0.0f,0.999f)) << ' '
	  << static_cast<int>(256.0f * Clamp<Float>(b,0.0f,0.999f)) << std::endl;
	return o;
}