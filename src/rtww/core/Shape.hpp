#pragma once

#include "Core.hpp"
#include "AABB.hpp"

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
};

class TTranslate : public Shape {
public:
	TTranslate(std::shared_ptr<Shape> p, const Vector3f& displacement)
		: ptr(p), offset(displacement) {}

	virtual bool Intersection(const Ray& r, Float tMin, Float tMax, IntersectionRecord& rec)const override;

	virtual bool BoundingBox(Float time0, Float time1, AABB& outputBox) const override;

public:
	std::shared_ptr<Shape> ptr;
	Vector3f offset;
};


bool TTranslate::Intersection(const Ray& r, Float tMin, Float tMax, IntersectionRecord& rec)const {
	Ray moved_r(r.origin - offset, r.direction, r.time);
	if (!ptr->Intersection(moved_r, tMin, tMax, rec))
		return false;

	rec.hitPoint += offset;
	rec.SetFaceNormal(moved_r, rec.normal);

	return true;
}


bool TTranslate::BoundingBox(Float time0, Float time1, AABB& outputBox) const {
	if (!ptr->BoundingBox(time0, time1, outputBox))
		return false;

	outputBox = AABB(
		outputBox.Min() + offset,
		outputBox.Max() + offset);

	return true;
}


class TRotateY : public Shape {
public:
	TRotateY(std::shared_ptr<Shape> p, double angle);
	virtual bool Intersection(const Ray& r, Float tMin, Float tMax, IntersectionRecord& rec)const override;
	virtual bool BoundingBox(Float time0, Float time1, AABB& outputBox) const override {
		outputBox = bbox;
		return hasbox;
	}

public:
	std::shared_ptr<Shape> ptr;
	Float sin_theta;
	Float cos_theta;
	bool hasbox;
	AABB bbox;
};


TRotateY::TRotateY(std::shared_ptr<Shape> p, Float angle) : ptr(p) {
	auto radians = Radians(angle);
	sin_theta = sin(radians);
	cos_theta = cos(radians);
	hasbox = ptr->BoundingBox(0, 1, bbox);

	Point3f min(Infinity, Infinity, Infinity);
	Point3f max(-Infinity, -Infinity, -Infinity);

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 2; k++) {
				auto x = i * bbox.Max().x + (1 - i)*bbox.Min().x;
				auto y = j * bbox.Max().y + (1 - j)*bbox.Min().y;
				auto z = k * bbox.Max().z + (1 - k)*bbox.Min().z;

				auto newx = cos_theta * x + sin_theta * z;
				auto newz = -sin_theta * x + cos_theta * z;

				Vector3f tester(newx, y, newz);

				for (int c = 0; c < 3; c++) {
					min[c] = fmin(min[c], tester[c]);
					max[c] = fmax(max[c], tester[c]);
				}
			}
		}
	}

	bbox = AABB(min, max);
}


bool TRotateY::Intersection(const Ray& r, Float tMin, Float tMax, IntersectionRecord& rec) const {
	auto origin = r.origin;
	auto direction = r.direction;

	origin[0] = cos_theta * r.origin[0] - sin_theta * r.origin[2];
	origin[2] = sin_theta * r.origin[0] + cos_theta * r.origin[2];

	direction[0] = cos_theta * r.direction[0] - sin_theta * r.direction[2];
	direction[2] = sin_theta * r.direction[0] + cos_theta * r.direction[2];

	Ray rotated_r(origin, direction, r.time);

	if (!ptr->Intersection(rotated_r, tMin, tMax, rec))
		return false;

	auto p = rec.hitPoint;
	auto normal = rec.normal;

	p[0] = cos_theta * rec.hitPoint[0] + sin_theta * rec.hitPoint[2];
	p[2] = -sin_theta * rec.hitPoint[0] + cos_theta * rec.hitPoint[2];

	normal[0] = cos_theta * rec.normal[0] + sin_theta * rec.normal[2];
	normal[2] = -sin_theta * rec.normal[0] + cos_theta * rec.normal[2];

	rec.hitPoint = p;
	rec.SetFaceNormal(rotated_r, normal);

	return true;
}