#if defined(_MSC_VER)
#define NOMINMAX
#pragma once
#endif

#ifndef RTWW_CORE_H
#define RTWW_CORE_H

#define RTWW_BEGIN namespace rtww{
#define RTWW_END }

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

#endif