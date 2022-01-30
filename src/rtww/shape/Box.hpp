#pragma once

#include "Rectangle.hpp"
#include "core/World.hpp"

class Box : public Shape {
public:
	Box() {}
	Box(std::shared_ptr<rtww::Transform> object2World, std::shared_ptr<rtww::Transform> world2Object, 
		Point3f p0, Point3f p1, std::shared_ptr<Material> mat);

	virtual bool Intersection(const Ray& r, Float tMin, Float tMax, IntersectionRecord& rec) const override;
	virtual bool BoundingBox(Float time0, Float time1, AABB& outputBox) const override {
		outputBox = AABB(pMin, pmax);
		return true;
	}

public:
	Point3f pMin, pmax;
	ShapesSet sides;
};

Box::Box(std::shared_ptr<rtww::Transform> object2World, std::shared_ptr<rtww::Transform> world2Object, 
	Point3f p0, Point3f p1, std::shared_ptr<Material> mat) {
	pMin = p0;
	pmax = p1;

	sides.Add(std::make_shared<RectangleXY>(object2World, world2Object, p0.x, p1.x, p0.y, p1.y, p1.z, mat));
	sides.Add(std::make_shared<RectangleXY>(object2World, world2Object, p0.x, p1.x, p0.y, p1.y, p0.z, mat));

	sides.Add(std::make_shared<RectangleXZ>(object2World, world2Object, p0.x, p1.x, p0.z, p1.z, p1.y, mat));
	sides.Add(std::make_shared<RectangleXZ>(object2World, world2Object, p0.x, p1.x, p0.z, p1.z, p0.y, mat));

	sides.Add(std::make_shared<RectangleYZ>(object2World, world2Object, p0.y, p1.y, p0.z, p1.z, p1.x, mat));
	sides.Add(std::make_shared<RectangleYZ>(object2World, world2Object, p0.y, p1.y, p0.z, p1.z, p0.x, mat));
}

bool Box::Intersection(const Ray & r, Float tMin, Float tMax, IntersectionRecord & rec) const {
	return sides.Intersection(r, tMin, tMax, rec);
}