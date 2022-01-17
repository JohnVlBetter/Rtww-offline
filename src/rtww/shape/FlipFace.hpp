#pragma once

#include "Rectangle.hpp"
#include "core/World.hpp"

class FlipFace : public Shape {
public:
	FlipFace(std::shared_ptr<Shape> p) : ptr(p) {}

	virtual bool Intersection(const Ray& r, Float tMin, Float tMax, IntersectionRecord& rec) const override {

		if (!ptr->Intersection(r, tMin, tMax, rec))
			return false;

		rec.isFrontFace = !rec.isFrontFace;
		return true;
	}

	virtual bool BoundingBox(Float time0, Float time1, AABB& outputBox) const override {
		return ptr->BoundingBox(time0, time1, outputBox);
	}
public:
	std::shared_ptr<Shape> ptr;
};