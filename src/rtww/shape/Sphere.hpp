#pragma once

#include "../core/Shape.hpp"

class Sphere :public Shape {
public:
	Sphere() {}
	Sphere(Point3f c, Float r) :center(c), radius(r) {}

	virtual bool Intersection(const Ray& r, Float tMin, Float tMax, IntersectionRecord& rec) const override;

public:
	Point3f center;
	Float radius;
};

bool Sphere::Intersection(const Ray & r, Float tMin, Float tMax, IntersectionRecord & rec) const {
	Vector3f o2c = r.origin - center;
	auto a = r.direction.LengthSquared();
	auto halfB = Dot(o2c, r.direction);
	auto c = o2c.LengthSquared() - radius * radius;
	auto delta = halfB * halfB - a * c;
	if (delta < 0) return false;
	auto sqrtDelta = sqrt(delta);

	auto root = (-halfB - sqrtDelta) / a;
	if (root < tMin || root > tMax) {
		root = (-halfB + sqrtDelta) / a;
		if (root < tMin || root > tMax)
			return false;
	}

	rec.time = root;
	rec.hitPoint = r.At(root);
	rec.normal = (rec.hitPoint - center) / radius;
	rec.SetFaceNormal(r, rec.normal);

	return true;
}