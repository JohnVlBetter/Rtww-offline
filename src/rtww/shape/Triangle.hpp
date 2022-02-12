#include "core/Shape.hpp"

class Triangle : public Shape {
public:
	Triangle(Vector3f p1, Vector3f p2, Vector3f p3, std::shared_ptr<Material> mat) :p1(p1), p2(p2), p3(p3), material(mat) {}
	virtual bool Intersection(const Ray& r, Float tMin, Float tMax, IntersectionRecord& rec) const override;
	virtual bool BoundingBox(Float time0, Float time1, AABB& outputBox) const override;
public:
	Vector3f p1,p2,p3;
	std::shared_ptr<Material> material;
};

bool Triangle::BoundingBox(Float time0, Float time1, AABB& outputBox) const {
	return false;
}

bool Triangle::Intersection(const Ray & r, Float tMin, Float tMax, IntersectionRecord & rec) const {
	float u, v;
	Vector3f o(r.origin.x, r.origin.y, r.origin.z);
	Vector3f e1 = p2 - p1;
	Vector3f e2 = p3 - p1;
	Vector3f s = o - p1;
	Vector3f s1 = Cross(r.direction, e2);
	Vector3f s2 = Cross(s, e1);
	float coeff = 1.0 / Dot(s1, e1);
	float t = coeff * Dot(s2, e2);
	float b1 = coeff * Dot(s1, s);
	float b2 = coeff * Dot(s2, r.direction);
	if (t >= 0 && b1 >= 0 && b2 >= 0 && (1 - b1 - b2) >= 0){
		u = b1;
		v = b2;
	}
	else return false;
	rec.time = t;
	rec.hitPoint = r.At(t);
	rec.normal = Cross(e2, e1).Normalize();
	rec.matPtr = material;
	rec.SetFaceNormal(r, rec.normal);
	rec.u = u;
	rec.v = v;
	return true;
}