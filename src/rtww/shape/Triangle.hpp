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

bool IntersectTriangle(const Vector3f& orig, const Vector3f& dir, const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, float* t, float* u, float* v) {
	Vector3 E1 = v1 - v0;
	Vector3 E2 = v2 - v0;
	Vector3 P = Cross(dir, E2);
	float det = Dot(E1, P);
	Vector3f T;
	if (det > 0) T = orig - v0;
	else {
		T = v0 - orig;
		det = -det;
	}

	if (det < 0.0001f) return false;
	*u = Dot(T, P);
	if (*u < 0.0f || *u > det) return false;

	Vector3f Q = Cross(T, E1);
	*v = Dot(dir, Q);
	if (*v < 0.0f || *u + *v > det) return false;
	*t = Dot(E2, Q);
	float fInvDet = 1.0f / det;
	*t *= fInvDet;
	*u *= fInvDet;
	*v *= fInvDet;

	return true;
}

bool Triangle::Intersection(const Ray & r, Float tMin, Float tMax, IntersectionRecord & rec) const {
	Vector3f o(r.origin.x, r.origin.y, r.origin.z);
	float t, u, v;
	bool result = IntersectTriangle(o, r.direction, p1, p2, p3, &t, &u, &v);
	rec.time = t;
	rec.hitPoint = r.At(t);
	rec.normal = Vector3f(0, 0, -1);
	rec.matPtr = material;
	rec.SetFaceNormal(r, rec.normal);
	rec.u = u;
	rec.v = v;
	return result;
}