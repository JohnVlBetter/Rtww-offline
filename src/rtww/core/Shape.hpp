#pragma once

#include "Core.hpp"
#include "AABB.hpp"
#include "Transform.hpp"

class Material;

struct IntersectionRecord {
	Point3f hitPoint;
	Vector3f normal;
	std::shared_ptr<Material> matPtr;
	Float time;
	Float u;
	Float v;
	bool isFrontFace;

	inline void SetFaceNormal(const Ray& r, const Vector3f& outwardNormal) {
		isFrontFace = Dot(r.direction, outwardNormal) < 0;
		normal = isFrontFace ? outwardNormal : -outwardNormal;
	}
};

class Shape {
public:
	virtual bool Intersection(const Ray& r, Float tMin, Float tMax, IntersectionRecord& rec)const = 0;
	virtual bool BoundingBox(Float time0, Float time1, AABB& outputBox) const = 0;
	virtual Float PDFValue(const Point3f& o, const Vector3f& v) const { return 0.0; }
	virtual Vector3f ShapeRandom(const Point3f& o) const { return Vector3f(1, 0, 0); }
protected:
	std::shared_ptr<rtww::Transform> object2World,world2Object;
};