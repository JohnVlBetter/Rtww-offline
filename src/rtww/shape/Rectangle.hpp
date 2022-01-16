#pragma once

#include "core/Shape.hpp"

class RectangleXY : public Shape {
public:
	RectangleXY() {}
	RectangleXY(Float x0, Float x1, Float y0, Float y1, Float z, std::shared_ptr<Material> mat)
		:x0(x0), x1(x1), y0(y0), y1(y1), z(z), material(mat) {}

	virtual bool Intersection(const Ray& r, Float tMin, Float tMax, IntersectionRecord& rec) const override;
	virtual bool BoundingBox(Float time0, Float time1, AABB& outputBox) const override {
		outputBox = AABB(Point3f(x0, y0, z - 0.0001f), Point3f(x1, y1, z + 0.0001f));
		return true;
	}

public:
	Float x0, x1, y0, y1, z;
	std::shared_ptr<Material> material;
};

bool RectangleXY::Intersection(const Ray & r, Float tMin, Float tMax, IntersectionRecord & rec) const {
	auto t = (z - r.origin.z) / r.direction.z;
	if (t < tMin || t > tMax)
		return false;
	auto x = r.origin.x + t * r.direction.x;
	auto y = r.origin.y + t * r.direction.y;
	if (x < x0 || x > x1 || y < y0 || y > y1)
		return false;
	rec.u = (x - x0) / (x1 - x0);
	rec.v = (y - y0) / (y1 - y0);
	rec.time = t;
	auto outward_Normal = Vector3f(0, 0, 1);
	rec.SetFaceNormal(r, outward_Normal);
	rec.matPtr = material;
	rec.hitPoint = r.At(t);
	return true;
}

class RectangleXZ : public Shape {
public:
	RectangleXZ() {}
	RectangleXZ(Float x0, Float x1, Float z0, Float z1, Float y, std::shared_ptr<Material> mat)
		:x0(x0), x1(x1), z0(z0), z1(z1), y(y), material(mat) {}

	virtual bool Intersection(const Ray& r, Float tMin, Float tMax, IntersectionRecord& rec) const override;
	virtual bool BoundingBox(Float time0, Float time1, AABB& outputBox) const override {
		outputBox = AABB(Point3f(x0, y - 0.0001f, z0), Point3f(x1, y + 0.0001f, z1));
		return true;
	}

public:
	Float x0, x1, z0, z1, y;
	std::shared_ptr<Material> material;
};

bool RectangleXZ::Intersection(const Ray & r, Float tMin, Float tMax, IntersectionRecord & rec) const {
	auto t = (y - r.origin.y) / r.direction.y;
	if (t < tMin || t > tMax)
		return false;
	auto x = r.origin.x + t * r.direction.x;
	auto z = r.origin.z + t * r.direction.z;
	if (x < x0 || x > x1 || z < z0 || z > z1)
		return false;
	rec.u = (x - x0) / (x1 - x0);
	rec.v = (z - z0) / (z1 - z0);
	rec.time = t;
	auto outward_Normal = Vector3f(0, 1, 0);
	rec.SetFaceNormal(r, outward_Normal);
	rec.matPtr = material;
	rec.hitPoint = r.At(t);
	return true;
}

class RectangleYZ : public Shape {
public:
	RectangleYZ() {}
	RectangleYZ(Float y0, Float y1, Float z0, Float z1, Float x, std::shared_ptr<Material> mat)
		:z0(z0), z1(z1), y0(y0), y1(y1), x(x), material(mat) {}

	virtual bool Intersection(const Ray& r, Float tMin, Float tMax, IntersectionRecord& rec) const override;
	virtual bool BoundingBox(Float time0, Float time1, AABB& outputBox) const override {
		outputBox = AABB(Point3f(x - 0.0001f, z0, y0), Point3f(x + 0.0001f, z1, y1));
		return true;
	}

public:
	Float z0, z1, y0, y1, x;
	std::shared_ptr<Material> material;
};

bool RectangleYZ::Intersection(const Ray & r, Float tMin, Float tMax, IntersectionRecord & rec) const {
	auto t = (x - r.origin.x) / r.direction.x;
	if (t < tMin || t > tMax)
		return false;
	auto z = r.origin.z + t * r.direction.z;
	auto y = r.origin.y + t * r.direction.y;
	if (y < y0 || y > y1 || z < z0 || z > z1)
		return false;
	rec.u = (y - y0) / (y1 - y0);
	rec.v = (z - z0) / (z1 - z0);
	rec.time = t;
	auto outward_Normal = Vector3f(1, 0, 0);
	rec.SetFaceNormal(r, outward_Normal);
	rec.matPtr = material;
	rec.hitPoint = r.At(t);
	return true;
}