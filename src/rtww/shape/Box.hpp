#pragma once

#include "Rectangle.hpp"
#include "core/World.hpp"

class Box : public Shape {
public:
	Box() {}
	Box(std::shared_ptr<Transform> transform, std::shared_ptr<Material> mat);

	virtual bool Intersection(const Ray& r, Float tMin, Float tMax, IntersectionRecord& rec) const override;
	virtual bool BoundingBox(Float time0, Float time1, AABB& outputBox) const override {
		auto center = transform->GetPosition();
		auto xl = transform->GetScale().x * 0.5f;
		auto yl = transform->GetScale().y * 0.5f;
		auto zl = transform->GetScale().z * 0.5f;
		outputBox = AABB(Point3f(center.x - xl, center.y - yl, center.z - zl), center + Point3f(center.x + xl, center.y + yl, center.z + zl));
		return true;
	}

public:
	ShapesSet sides;
};

Box::Box(std::shared_ptr<Transform> transform, std::shared_ptr<Material> mat) {
	this->transform = transform;
	
	sides.Add(std::make_shared<RectangleXY>(std::make_shared<Transform>(Point3f(0, 0, 0.5), Vector3f(1.0f, 1.0f, 0.03f), Vector3f()), mat));
	sides.Add(std::make_shared<RectangleXY>(std::make_shared<Transform>(Point3f(0, 0, -0.5), Vector3f(1.0f, 1.0f, 0.03f), Vector3f()), mat));
																							 	
	sides.Add(std::make_shared<RectangleXZ>(std::make_shared<Transform>(Point3f(0, 0.5, 0), Vector3f(1.0f, 0.03f, 1.0f), Vector3f()), mat));
	sides.Add(std::make_shared<RectangleXZ>(std::make_shared<Transform>(Point3f(0, -0.5, 0), Vector3f(1.0f, 0.03f, 1.0f), Vector3f()), mat));
																		
	sides.Add(std::make_shared<RectangleYZ>(std::make_shared<Transform>(Point3f(0.5, 0, 0), Vector3f(0.03f, 1.0f, 1.0f), Vector3f()), mat));
	sides.Add(std::make_shared<RectangleYZ>(std::make_shared<Transform>(Point3f(-0.5, 0, 0), Vector3f(0.03f, 1.0f, 1.0f), Vector3f()), mat));
}

bool Box::Intersection(const Ray & r, Float tMin, Float tMax, IntersectionRecord & rec) const {
	Ray ray = transform->GetWorld2ObjectMatrix()(r);
	auto hit = sides.Intersection(ray, tMin, tMax, rec);
	auto outwardNormal = transform->GetObject2WorldMatrix()(rec.normal);
	rec.SetFaceNormal(ray, outwardNormal);
	rec.hitPoint = transform->GetObject2WorldMatrix()(ray.At(rec.time));
	return hit;
}