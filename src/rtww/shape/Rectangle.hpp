#pragma once

#include "core/Shape.hpp"

class RectangleXY : public Shape {
public:
	RectangleXY() {}
	RectangleXY(std::shared_ptr<rtww::Transform> object2World, std::shared_ptr<rtww::Transform> world2Object, 
		Float x0, Float x1, Float y0, Float y1, Float z, std::shared_ptr<Material> mat)
		:x0(x0), x1(x1), y0(y0), y1(y1), z(z), material(mat) {
		this->object2World = object2World;
		this->world2Object = world2Object;
	}

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
	Ray ray = (*world2Object)(r);
	auto t = (z - ray.origin.z) / ray.direction.z;
	if (t < tMin || t > tMax)
		return false;
	auto x = ray.origin.x + t * ray.direction.x;
	auto y = ray.origin.y + t * ray.direction.y;
	if (x < x0 || x > x1 || y < y0 || y > y1)
		return false;
	rec.u = (x - x0) / (x1 - x0);
	rec.v = (y - y0) / (y1 - y0);
	rec.time = t;
	auto outwardNormal = (*object2World)(Vector3f(0, 0, 1));
	rec.SetFaceNormal(ray, outwardNormal);
	rec.matPtr = material;
	rec.hitPoint = (*object2World)(ray.At(t));;
	return true;
}

class RectangleXZ : public Shape {
public:
	RectangleXZ() {}
	RectangleXZ(std::shared_ptr<rtww::Transform> object2World, std::shared_ptr<rtww::Transform> world2Object,
		Float x0, Float x1, Float z0, Float z1, Float y, std::shared_ptr<Material> mat)
		:x0(x0), x1(x1), z0(z0), z1(z1), y(y), material(mat) {
		this->object2World = object2World;
		this->world2Object = world2Object;
	}

	virtual bool Intersection(const Ray& r, Float tMin, Float tMax, IntersectionRecord& rec) const override;
	virtual bool BoundingBox(Float time0, Float time1, AABB& outputBox) const override {
		outputBox = AABB(Point3f(x0, y - 0.0001f, z0), Point3f(x1, y + 0.0001f, z1));
		return true;
	}
	virtual Float PDFValue(const Point3f& o, const Vector3f& v) const {
		IntersectionRecord rec;
		if (!this->Intersection(Ray(o, v), 0.001, Infinity, rec))
			return 0;

		auto area = (x1 - x0)*(z1 - z0);
		auto distanceSquared = rec.time * rec.time * v.LengthSquared();
		auto cosine = fabs(Dot(v, rec.normal) / v.Length());

		return distanceSquared / (cosine * area);
	}
	virtual Vector3f ShapeRandom(const Point3f& o) const {
		auto randomPoint = Point3f(Random<Float>(x0, x1), y, Random<Float>(z0, z1));
		return randomPoint - o;
	}

public:
	Float x0, x1, z0, z1, y;
	std::shared_ptr<Material> material;
};

bool RectangleXZ::Intersection(const Ray & r, Float tMin, Float tMax, IntersectionRecord & rec) const {
	Ray ray = (*world2Object)(r);
	auto t = (y - ray.origin.y) / ray.direction.y;
	if (t < tMin || t > tMax)
		return false;
	auto x = ray.origin.x + t * ray.direction.x;
	auto z = ray.origin.z + t * ray.direction.z;
	if (x < x0 || x > x1 || z < z0 || z > z1)
		return false;
	rec.u = (x - x0) / (x1 - x0);
	rec.v = (z - z0) / (z1 - z0);
	rec.time = t;
	auto outwardNormal = (*object2World)(Vector3f(0, 1, 0));
	rec.SetFaceNormal(ray, outwardNormal);
	rec.matPtr = material;
	rec.hitPoint = (*object2World)(ray.At(t));
	return true;
}

class RectangleYZ : public Shape {
public:
	RectangleYZ() {}
	RectangleYZ(std::shared_ptr<rtww::Transform> object2World, std::shared_ptr<rtww::Transform> world2Object,
		Float y0, Float y1, Float z0, Float z1, Float x, std::shared_ptr<Material> mat)
		:z0(z0), z1(z1), y0(y0), y1(y1), x(x), material(mat) {
		this->object2World = object2World;
		this->world2Object = world2Object;
	}

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
	Ray ray = (*world2Object)(r);
	auto t = (x - ray.origin.x) / ray.direction.x;
	if (t < tMin || t > tMax)
		return false;
	auto z = ray.origin.z + t * ray.direction.z;
	auto y = ray.origin.y + t * ray.direction.y;
	if (y < y0 || y > y1 || z < z0 || z > z1)
		return false;
	rec.u = (y - y0) / (y1 - y0);
	rec.v = (z - z0) / (z1 - z0);
	rec.time = t;
	auto outward_Normal = (*object2World)(Vector3f(1, 0, 0));
	rec.SetFaceNormal(ray, outward_Normal);
	rec.matPtr = material;
	rec.hitPoint = (*object2World)(ray.At(t));
	return true;
}