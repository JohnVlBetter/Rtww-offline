#pragma once;
#include "Core.hpp"

class AABB {
public:
	AABB() {}
	AABB(const Point3f& p1, const Point3f& p2):boundingBox(Bounds3f(p1,p2)) {}

	Point3f Min() const { return boundingBox.pMin; }
	Point3f Max() const { return boundingBox.pMax; }

	bool Intersection(const Ray& r, Float tMin, Float tMax) const {
		for (int i = 0; i < 3; ++i) {
			auto invD = 1.0f / r.direction[i];
			auto t0 = (Min()[i] - r.origin[i]) * invD;
			auto t1 = (Max()[i] - r.origin[i]) * invD;
			if (invD < 0.0f) std::swap(t0, t1);
			tMin = t0 > tMin ? t0 : tMin;
			tMax = t1 < tMax ? t1 : tMax;
			if (tMax <= tMin) return false;
		}
		return true;
	}

public:
	Bounds3f boundingBox;
};

AABB SurroundingBox(AABB box0, AABB box1) {
	Point3f small(fmin(box0.Min().x, box1.Min().x),
		fmin(box0.Min().y, box1.Min().y),
		fmin(box0.Min().z, box1.Min().z));

	Point3f big(fmax(box0.Max().x, box1.Max().x),
		fmax(box0.Max().y, box1.Max().y),
		fmax(box0.Max().z, box1.Max().z));

	return AABB(small, big);
}