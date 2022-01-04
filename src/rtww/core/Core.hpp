#pragma once

#define USE_DOUBLE
#include "hsm.hpp"
using namespace hsm;

typedef Vector3<Float> Color;

std::ostream& operator << (std::ostream &o, const Color c) {
	o << static_cast<int>(255.0f * c.x) << ' '
	  << static_cast<int>(255.0f * c.y) << ' '
	  << static_cast<int>(255.0f * c.z) << std::endl;
	return o;
}