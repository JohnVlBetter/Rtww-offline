#pragma once

#include "core/Shape.hpp"

class RectangleXY : public Shape {
public:
	RectangleXY() {}
	RectangleXY(std::shared_ptr<rtww::Transform> object2World, std::shared_ptr<rtww::Transform> world2Object, 
		std::shared_ptr<Material> mat) : material(mat) {
		this->object2World = object2World;
		this->world2Object = world2Object;
	}

	virtual bool Intersection(const Ray& r, Float tMin, Float tMax, IntersectionRecord& rec) const override;
	virtual bool BoundingBox(Float time0, Float time1, AABB& outputBox) const override {
		auto center = Point3f(object2World->GetMatrix().data[0][3], object2World->GetMatrix().data[1][3], object2World->GetMatrix().data[2][3]);
		auto xl = object2World->GetMatrix().data[0][0] * 0.5f;
		auto yl = object2World->GetMatrix().data[1][1] * 0.5f;
		outputBox = AABB(Point3f(center.x - xl, center.y - yl, center.z - 0.0001f), Point3f(center.x + xl, center.y + yl, center.z + 0.0001f));
		return true;
	}

public:
	std::shared_ptr<Material> material;
};

bool RectangleXY::Intersection(const Ray & r, Float tMin, Float tMax, IntersectionRecord & rec) const {
	Ray ray = (*world2Object)(r);
	auto t = -ray.origin.z / ray.direction.z;
	if (t < tMin || t > tMax)
		return false;
	auto x = ray.origin.x + t * ray.direction.x;
	auto y = ray.origin.y + t * ray.direction.y;
	if (x < -0.5f || x > 0.5f || y < -0.5f || y > 0.5f)
		return false;
	rec.u = x + 0.5f;
	rec.v = y + 0.5f;
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
		std::shared_ptr<Material> mat) : material(mat) {
		this->object2World = object2World;
		this->world2Object = world2Object;
		auto center = Point3f(object2World->GetMatrix().data[0][3], object2World->GetMatrix().data[1][3], object2World->GetMatrix().data[2][3]);
		auto xl = object2World->GetMatrix().data[0][0] * 0.5f;
		auto zl = object2World->GetMatrix().data[2][2] * 0.5f;
		this->x0 = center.x - xl;
		this->x1 = center.x + xl;
		this->z0 = center.z - zl;
		this->z1 = center.z + zl;
		this->y = center.y;
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
	std::shared_ptr<Material> material;

private:
	Float x0, x1, z0, z1, y;
};

bool RectangleXZ::Intersection(const Ray & r, Float tMin, Float tMax, IntersectionRecord & rec) const {
	Ray ray = (*world2Object)(r);
	auto t = -ray.origin.y / ray.direction.y;
	if (t < tMin || t > tMax)
		return false;
	auto x = ray.origin.x + t * ray.direction.x;
	auto z = ray.origin.z + t * ray.direction.z;
	if (x < -0.5f || x > 0.5f || z < -0.5f || z > 0.5f)
		return false;
	rec.u = x + 0.5f;
	rec.v = z + 0.5f;
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
		std::shared_ptr<Material> mat) : material(mat) {
		this->object2World = object2World;
		this->world2Object = world2Object;
	}

	virtual bool Intersection(const Ray& r, Float tMin, Float tMax, IntersectionRecord& rec) const override;
	virtual bool BoundingBox(Float time0, Float time1, AABB& outputBox) const override {
		auto center = Point3f(object2World->GetMatrix().data[0][3], object2World->GetMatrix().data[1][3], object2World->GetMatrix().data[2][3]);
		auto yl = object2World->GetMatrix().data[1][1] * 0.5f;
		auto zl = object2World->GetMatrix().data[2][2] * 0.5f;
		outputBox = AABB(Point3f(center.x - 0.0001f, center.y - yl, center.z - zl), Point3f(center.x + 0.0001f, center.y + yl, center.z + zl));
		return true;
	}

public:
	std::shared_ptr<Material> material;
};

bool RectangleYZ::Intersection(const Ray & r, Float tMin, Float tMax, IntersectionRecord & rec) const {
	Ray ray = (*world2Object)(r);
	auto t = -ray.origin.x / ray.direction.x;
	if (t < tMin || t > tMax)
		return false;
	auto z = ray.origin.z + t * ray.direction.z;
	auto y = ray.origin.y + t * ray.direction.y;
	if (y < -0.5f || y > 0.5f || z < -0.5f || z > 0.5f)
		return false;
	rec.u = y + 0.5f;
	rec.v = z + 0.5f;
	rec.time = t;
	auto outward_Normal = (*object2World)(Vector3f(1, 0, 0));
	rec.SetFaceNormal(ray, outward_Normal);
	rec.matPtr = material;
	rec.hitPoint = (*object2World)(ray.At(t));
	return true;
}