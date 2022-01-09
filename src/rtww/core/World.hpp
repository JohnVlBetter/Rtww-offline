#pragma once

#include "Shape.hpp"
#include "AABB.hpp"

class ShapesSet : public Shape{
public:
	ShapesSet() {}
	ShapesSet(std::shared_ptr<Shape> object) { Add(object); }

	void Add(std::shared_ptr<Shape> object) { objects.push_back(object); }
	void Clear() { objects.clear(); }

	virtual bool Intersection(const Ray& r, Float tMin, Float tMax, IntersectionRecord& rec) const override;
	virtual bool BoundingBox(Float time0, Float time1, AABB& outputBox) const override;
public:
	std::vector<std::shared_ptr<Shape>> objects;
};

bool ShapesSet::BoundingBox(Float time0, Float time1, AABB& outputBox) const {
	if (objects.empty()) return false;

	AABB tempBox;
	bool first = true;

	for (const auto& object : objects) {
		if (!object->BoundingBox(time0, time1, tempBox)) return false;
		outputBox = first ? tempBox : SurroundingBox(outputBox, tempBox);
		first = false;
	}

	return true;
}

bool ShapesSet::Intersection(const Ray& r, Float tMin, Float tMax, IntersectionRecord& rec) const {
	IntersectionRecord tRec;
	auto hitAnything = false;
	auto closest = tMax;
	for (const auto& object : objects) {
		if (object->Intersection(r, tMin, closest, tRec)) {
			closest = tRec.time;
			rec = tRec;
			hitAnything = true;
		}
	}
	return hitAnything;
}