#pragma once

#include "core/Shape.hpp"
#include "core/Transform.hpp"
#include "core/OrthonormalBasis.hpp"

class Sphere :public Shape {
public:
	Sphere() {
	}
	Sphere(std::shared_ptr<Transform> transform, std::shared_ptr<Material> mat) : material(mat){
		this->transform = transform;
	}

	virtual bool Intersection(const Ray& r, Float tMin, Float tMax, IntersectionRecord& rec) const override;
	virtual bool BoundingBox(Float time0, Float time1, AABB& outputBox) const override;
	virtual Float PDFValue(const Point3f& o, const Vector3f& v) const;
	virtual Vector3f ShapeRandom(const Point3f& o) const;

private:
	static void GetUV(const Point3f& p, Float& u, Float& v) {
		auto theta = acos(-p.y);
		auto phi = atan2(-p.z, p.x) + Pi;

		u = phi / (2 * Pi);
		v = theta / Pi;
	}

public:
	std::shared_ptr<Material> material;
};

std::shared_ptr<Sphere> CreateSphere(const Point3f& position, const Vector3f& scale, const Vector3f& rotation, std::shared_ptr<Material> mat) {
	return std::make_shared<Sphere>(std::make_shared<Transform>(position, scale, rotation), mat);
}

bool Sphere::BoundingBox(Float time0, Float time1, AABB& outputBox) const {
	auto center = transform->GetPosition();
	auto radius = transform->GetScale()[0];
	outputBox = AABB(center - Vector3f(radius, radius, radius), center + Vector3f(radius, radius, radius));
	return true;
}

Float Sphere::PDFValue(const Point3f & o, const Vector3f & v) const{
	IntersectionRecord rec;
	if (!Intersection(Ray(o, v), 0.001, Infinity, rec))
		return 0;
	auto center = transform->GetPosition();
	auto radius = transform->GetScale()[0];
	auto cosThetaMax = sqrt(1 - radius * radius / (center - o).LengthSquared());
	auto solidAngle = 2 * Pi*(1 - cosThetaMax);
	return 1 / solidAngle;
}

Vector3f Sphere::ShapeRandom(const Point3f & o) const{
	auto center = transform->GetPosition();
	auto radius = transform->GetScale()[0];
	Vector3f dir = center - o;
	auto distanceSquared = dir.LengthSquared();
	OrthonormalBasis onb;
	onb.BuildFromW(dir);
	return onb.Local(Random2Sphere(radius, distanceSquared));
}

bool Sphere::Intersection(const Ray & r, Float tMin, Float tMax, IntersectionRecord & rec) const {
	Ray ray = transform->GetWorld2ObjectMatrix()(r);
	Vector3f o2c = Convert(ray.origin);
	auto a = ray.direction.LengthSquared();
	auto halfB = Dot(o2c, ray.direction);
	auto c = o2c.LengthSquared() - 1;
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
	rec.hitPoint = transform->GetObject2WorldMatrix()(ray.At(root));
	rec.normal = (rec.hitPoint - transform->GetObject2WorldMatrix()(Point3f()));
	rec.SetFaceNormal(ray, rec.normal);
	GetUV(Point3f(rec.normal.x, rec.normal.y, rec.normal.z), rec.u, rec.v);
	rec.matPtr = material;

	return true;
}