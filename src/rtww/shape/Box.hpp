#pragma once

#include "Rectangle.hpp"
#include "core/World.hpp"

class Box : public Shape {
public:
	Box() {}
	Box(std::shared_ptr<rtww::Transform> object2World, std::shared_ptr<rtww::Transform> world2Object, std::shared_ptr<Material> mat);

	virtual bool Intersection(const Ray& r, Float tMin, Float tMax, IntersectionRecord& rec) const override;
	virtual bool BoundingBox(Float time0, Float time1, AABB& outputBox) const override {
		auto center = Point3f(object2World->GetMatrix().data[0][3], object2World->GetMatrix().data[1][3], object2World->GetMatrix().data[2][3]);
		auto xl = object2World->GetMatrix().data[0][0] * 0.5f;
		auto yl = object2World->GetMatrix().data[1][1] * 0.5f;
		auto zl = object2World->GetMatrix().data[2][2] * 0.5f;
		outputBox = AABB(Point3f(center.x - xl, center.y - yl, center.z - zl), center + Point3f(center.x + xl, center.y + yl, center.z + zl));
		return true;
	}

public:
	ShapesSet sides;
};

Box::Box(std::shared_ptr<rtww::Transform> object2World, std::shared_ptr<rtww::Transform> world2Object, std::shared_ptr<Material> mat) {
	auto center = Point3f(object2World->GetMatrix().data[0][3], object2World->GetMatrix().data[1][3], object2World->GetMatrix().data[2][3]);
	auto xl = object2World->GetMatrix().data[0][0] * 0.5f;
	auto yl = object2World->GetMatrix().data[1][1] * 0.5f;
	auto zl = object2World->GetMatrix().data[2][2] * 0.5f;
	
	auto translate = hsm::Translate(Vector3f(center.x, center.y, center.z - zl));
	auto tMat = object2World->GetMatrix();
	tMat = translate * tMat;
	sides.Add(std::make_shared<RectangleXY>(std::make_shared<rtww::Transform>(tMat), std::make_shared<rtww::Transform>(tMat.Inverse()), mat));

	translate = hsm::Translate(Vector3f(center.x, center.y, center.z + zl));
	tMat = object2World->GetMatrix();
	tMat = translate * tMat;
	sides.Add(std::make_shared<RectangleXY>(std::make_shared<rtww::Transform>(tMat), std::make_shared<rtww::Transform>(tMat.Inverse()), mat));

	translate = hsm::Translate(Vector3f(center.x, center.y - yl , center.z));
	tMat = object2World->GetMatrix();
	tMat = translate * tMat;
	sides.Add(std::make_shared<RectangleXZ>(std::make_shared<rtww::Transform>(tMat), std::make_shared<rtww::Transform>(tMat.Inverse()), mat));

	translate = hsm::Translate(Vector3f(center.x, center.y + yl, center.z));
	tMat = object2World->GetMatrix();
	tMat = translate * tMat;
	sides.Add(std::make_shared<RectangleXZ>(std::make_shared<rtww::Transform>(tMat), std::make_shared<rtww::Transform>(tMat.Inverse()), mat));

	translate = hsm::Translate(Vector3f(center.x - xl, center.y, center.z));
	tMat = object2World->GetMatrix();
	tMat = translate * tMat;
	sides.Add(std::make_shared<RectangleYZ>(std::make_shared<rtww::Transform>(tMat), std::make_shared<rtww::Transform>(tMat.Inverse()), mat));

	translate = hsm::Translate(Vector3f(center.x + xl, center.y, center.z));
	tMat = object2World->GetMatrix();
	tMat = translate * tMat;
	sides.Add(std::make_shared<RectangleYZ>(std::make_shared<rtww::Transform>(tMat), std::make_shared<rtww::Transform>(tMat.Inverse()), mat));
}

bool Box::Intersection(const Ray & r, Float tMin, Float tMax, IntersectionRecord & rec) const {
	return sides.Intersection(r, tMin, tMax, rec);
}