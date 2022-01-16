#pragma once

#include "Core.hpp"

class OrthonormalBasis {
public:
	OrthonormalBasis() {}
	OrthonormalBasis(const Vector3f& normal);

	Vector3f Local(Float a, Float b, Float c) const { return a * xAxis + b * yAxis + c * zAxis; }
	Vector3f Local(const Vector3f& v) const { return v.x * xAxis + v.y * yAxis + v.z * zAxis; }

public:
	Vector3f xAxis, yAxis, zAxis;
};

OrthonormalBasis::OrthonormalBasis(const Vector3f& normal) {
	zAxis = normal.Normalize();
	Vector3f a = (fabs(zAxis.x) > 0.9f) ? Vector3f(0, 1, 0) : Vector3f(1, 0, 0);
	yAxis = Cross(zAxis, a).Normalize();
	xAxis = Cross(zAxis, yAxis);
}