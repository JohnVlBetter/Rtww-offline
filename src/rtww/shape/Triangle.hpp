#pragma once
#include "core/Shape.hpp"
#include "core/Mesh.hpp"

class Triangle : public Shape {
public:
	Triangle(const std::shared_ptr<Mesh> &m, int triangleIndex, std::shared_ptr<Material> mat)
		:mesh(m), material(mat) {
		vertexIndices = &mesh->vertexIndices[3 * triangleIndex];
	}

	virtual bool Intersection(const Ray& r, Float tMin, Float tMax, IntersectionRecord& rec) const override;
	virtual bool BoundingBox(Float time0, Float time1, AABB& outputBox) const override;

private:
	void GetUV(Point2f uv[3]) const {
		if (mesh->uvs) {
			uv[0] = mesh->uvs[vertexIndices[0]];
			uv[1] = mesh->uvs[vertexIndices[1]];
			uv[2] = mesh->uvs[vertexIndices[2]];
		}
		else {
			uv[0] = Point2f(0, 0);
			uv[1] = Point2f(1, 0);
			uv[2] = Point2f(1, 1);
		}
	}

public:
	const int* vertexIndices;
	std::shared_ptr<Material> material;
	std::shared_ptr<Mesh> mesh;
};

bool Triangle::BoundingBox(Float time0, Float time1, AABB& outputBox) const {
	return false;
}

bool Triangle::Intersection(const Ray & r, Float tMin, Float tMax, IntersectionRecord & rec) const {
	float u, v;
	Vector3f e1 = mesh->vertices[vertexIndices[1]] - mesh->vertices[vertexIndices[0]];
	Vector3f e2 = mesh->vertices[vertexIndices[2]] - mesh->vertices[vertexIndices[0]];
	Vector3f s = r.origin - mesh->vertices[vertexIndices[1]];
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

std::vector<std::shared_ptr<Shape>> CreateMesh(std::shared_ptr<Transform> transform,
	int nTriangles, const int *vertexIndices, int nVertices, const Point3f *v,
	const Vector3f *n, const Point2f *uv, std::shared_ptr<Material> mat) {
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(
		transform, nTriangles, vertexIndices, nVertices, v, n, uv);
	std::vector<std::shared_ptr<Shape>> triangles;
	triangles.reserve(nTriangles);
	for (int i = 0; i < nTriangles; ++i)
		triangles.push_back(std::make_shared<Triangle>(mesh, i, mat));
	return triangles;
}