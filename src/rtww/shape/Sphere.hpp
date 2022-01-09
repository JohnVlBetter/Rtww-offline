#pragma once

#include "../core/Shape.hpp"

class Sphere :public Shape {
public:
	Sphere() {}
	Sphere(Point3f c, Float r, std::shared_ptr<Material> mat) :center(c), radius(r), material(mat) {}

	virtual bool Intersection(const Ray& r, Float tMin, Float tMax, IntersectionRecord& rec) const override;
	virtual bool BoundingBox(Float time0, Float time1, AABB& outputBox) const override;

public:
	Point3f center;
	Float radius;
	std::shared_ptr<Material> material;
};

bool Sphere::BoundingBox(Float time0, Float time1, AABB& outputBox) const {
	outputBox = AABB(center - Vector3f(radius, radius, radius), center + Vector3f(radius, radius, radius));
	return true;
}

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
	rec.matPtr = material;

	return true;
}