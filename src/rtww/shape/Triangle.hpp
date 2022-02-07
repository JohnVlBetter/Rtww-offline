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

bool ray_triangle_intersect_mt(const Vector3f& O, const Vector3f& D, const Vector3f& p0, const Vector3f& p1, const Vector3f& p2, float &t, float &u, float &v) {
	Vector3f e1 = p1 - p0;
	Vector3f e2 = p2 - p0;
	Vector3f pvec = Cross(D, e2);
	Float det = Dot(e1, pvec);
	if (det < std::numeric_limits<Float>::epsilon()) return false;

	Vector3f tvec = O - p0;
	u = Dot(tvec, pvec);
	u *= (1. / det);
	if (u < 0 || u > 1) return false;

	Vector3f qvec = Cross(tvec, e1);
	v = Dot(D, qvec);
	v *= (1. / det);
	if (v < 0 || u + v > 1) return false;

	t = Dot(e2, qvec);
	t *= (1. / det);
	return t > std::numeric_limits<Float>::epsilon();
}

bool Triangle::Intersection(const Ray & r, Float tMin, Float tMax, IntersectionRecord & rec) const {
	Vector3f o(r.origin.x, r.origin.y, r.origin.z);
	float t, u, v;
	bool result = ray_triangle_intersect_mt(o, r.direction, p1, p2, p3, t, u, v);
	rec.time = t;
	rec.hitPoint = r.At(t);
	rec.normal = Vector3f(0, 0, -1);
	rec.matPtr = material;
	rec.SetFaceNormal(r, rec.normal);
	rec.u = u;
	rec.v = v;
	return result;
}