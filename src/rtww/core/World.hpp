#pragma once

#include "Shape.hpp"
#include <vector>

class ShapesSet : public Shape{
public:
	ShapesSet() {}
	ShapesSet(std::shared_ptr<Shape> object) {}

	void Add(std::shared_ptr<Shape> object) { objects.push_back(object); }
	void Clear() { objects.clear(); }

	virtual bool Intersection(const Ray& r, Float tMin, Float tMax, IntersectionRecord& rec) const override;
public:
	std::vector<std::shared_ptr<Shape>> objects;
};

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