#pragma once

#include "../core/World.hpp"
#include "../core/Material.hpp"
#include "../core/Texture.hpp"

class Medium : public Shape {
public:
	Medium(std::shared_ptr<Shape> b, double d, std::shared_ptr<Texture> a)
		: boundary(b), negInvDensity(-1 / d), phaseFunc(std::make_shared<Isotropic>(a)) {}
	Medium(std::shared_ptr<Shape> b, double d, Color c)
		: boundary(b), negInvDensity(-1 / d), phaseFunc(std::make_shared<Isotropic>(c)) {}

	virtual bool Intersection(const Ray& r, Float tMin, Float tMax, IntersectionRecord& rec) const override;
	virtual bool BoundingBox(Float time0, Float time1, AABB& outputBox) const override {
		return boundary->BoundingBox(time0, time1, outputBox);
	}

public:
	std::shared_ptr<Material> phaseFunc;
	std::shared_ptr<Shape> boundary;
	Float negInvDensity;
};

bool Medium::Intersection(const Ray & r, Float tMin, Float tMax, IntersectionRecord & rec) const {
	const bool enableDebug = false;
	const bool debugging = enableDebug && Random<Float>() < 0.00001;

	IntersectionRecord rec1, rec2;

	if (!boundary->Intersection(r, -Infinity, Infinity, rec1))
		return false;

	if (!boundary->Intersection(r, rec1.time + 0.0001, Infinity, rec2))
		return false;

	if (debugging) std::cerr << "\nt_min=" << rec1.time << ", t_max=" << rec2.time << '\n';

	if (rec1.time < tMin ) rec1.time = tMin;
	if (rec2.time > tMax) rec2.time = tMax;

	if (rec1.time >= rec2.time)
		return false;

	if (rec1.time < 0)
		rec1.time = 0;

	const auto rayLength = r.direction.Length();
	const auto distanceInsideBoundary = (rec2.time - rec1.time) * rayLength;
	const auto hitDistance = negInvDensity * log(Random<Float>());

	if (hitDistance > distanceInsideBoundary)
		return false;

	rec.time = rec1.time + hitDistance / rayLength;
	rec.hitPoint = r.At(rec.time);

	if (debugging) {
		std::cerr << "hit_distance = " << hitDistance << '\n'
			<< "rec.t = " << rec.time << '\n'
			<< "rec.p = " << rec.hitPoint << '\n';
	}

	rec.normal = Vector3f(1, 0, 0);  // arbitrary
	rec.isFrontFace = true;     // arbitrary
	rec.matPtr = phaseFunc;

	return true;
}