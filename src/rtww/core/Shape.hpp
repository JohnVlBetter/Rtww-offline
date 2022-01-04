#pragma once

#include "Core.hpp"

struct IntersectionRecord {
	Point3f hitPoint;
	Vector3f normal;
	Float time;
	bool isFrontFace;

	inline void SetFaceNormal(const Ray& r, const Vector3f& outwardNormal) {
		isFrontFace = Dot(r.direction, outwardNormal) < 0;
		normal = isFrontFace ? outwardNormal : -outwardNormal;
	}
};

class Shape {
public:
	virtual bool Intersection(const Ray& r, Float tMin, Float tMax, IntersectionRecord& rec)const = 0;
};