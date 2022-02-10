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

bool rayTriangleIntersect(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, const Vector3f& orig,
	const Vector3f& dir, float& tnear, float& u, float& v)
{
	bool isIn = false;
	Vector3f E1 = v1 - v0;
	Vector3f E2 = v2 - v0;
	Vector3f S = orig - v0;
	Vector3f S1 = Cross(dir, E2);
	Vector3f S2 = Cross(S, E1);
	float coeff = 1.0 / Dot(S1, E1); // 共同系数
	float t = coeff * Dot(S2, E2);
	float b1 = coeff * Dot(S1, S);
	float b2 = coeff * Dot(S2, dir);
	if (t >= 0 && b1 >= 0 && b2 >= 0 && (1 - b1 - b2) >= 0)
	{
		isIn = true;
		tnear = t;
		u = b1;
		v = b2;
	}

	return isIn;
}


bool Triangle::Intersection(const Ray & r, Float tMin, Float tMax, IntersectionRecord & rec) const {
	Vector3f o(r.origin.x, r.origin.y, r.origin.z);
	float t, u, v;
	bool result = rayTriangleIntersect(p1, p2, p3, o, r.direction, t, u, v);
	rec.time = t;
	rec.hitPoint = r.At(t);
	Vector3 E1 = p2 - p1;
	Vector3 E2 = p3 - p1;
	rec.normal = Vector3f(0,0,-1);
	rec.matPtr = material;
	rec.SetFaceNormal(r, rec.normal);
	rec.u = u;
	rec.v = v;
	return result;
}