#pragma once

#define USE_DOUBLE
#include "hsm.hpp"
using namespace hsm;
#include <vector>

Vector3i ConvertColor(const Color& c, int samples) {
	auto r = c.x;
	auto g = c.y;
	auto b = c.z;

	if (r != r) r = 0.0;
	if (g != g) g = 0.0;
	if (b != b) b = 0.0;

	auto scale = 1.0f / samples;
	r = sqrt(r * scale);
	g = sqrt(g * scale);
	b = sqrt(b * scale);

	return Vector3i(static_cast<int>(256.0f * Clamp<Float>(r, 0.0f, 0.999f)),
		static_cast<int>(256.0f * Clamp<Float>(g, 0.0f, 0.999f)), static_cast<int>(256.0f * Clamp<Float>(b, 0.0f, 0.999f)));
}