#pragma once
#include "core/Shape.hpp"
#include "core/Mesh.hpp"

class Triangle : public Shape {
public:
	Triangle(const std::shared_ptr<Mesh> &m, int triangleIndex, std::shared_ptr<Material> mat)
		:mesh(m), material(mat) {
		vertexIndices = &mesh->vertexIndices[3 * triangleIndex];
		//std::cerr << mesh->vertices[vertexIndices[0]] << mesh->vertices[vertexIndices[1]] << mesh->vertices[vertexIndices[2]] << std::endl;
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
	const Point3f &p0 = mesh->vertices[vertexIndices[0]];
	const Point3f &p1 = mesh->vertices[vertexIndices[1]];
	const Point3f &p2 = mesh->vertices[vertexIndices[2]];
	auto b3 = Union(Bounds3f(p0, p1), p2);
	outputBox = AABB(b3.pMin,b3.pMax);
	return true;
}

bool Triangle::Intersection(const Ray & r, Float tMin, Float tMax, IntersectionRecord & rec) const {
	float u, v;
	auto p0 = mesh->vertices[vertexIndices[0]];
	auto p1 = mesh->vertices[vertexIndices[1]];
	auto p2 = mesh->vertices[vertexIndices[2]];
	Vector3f e1 = p1 - p0;
	Vector3f e2 = p2 - p0;
	Vector3f s = r.origin - mesh->vertices[vertexIndices[0]];
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
	rec.normal = Cross(-e1, -e2).Normalize();
	rec.matPtr = material;
	rec.SetFaceNormal(r, rec.normal);
	Point2f uv[3];
	GetUV(uv);
	Vector3f f1 = p0 - rec.hitPoint;
	Vector3f f2 = p1 - rec.hitPoint;
	Vector3f f3 = p2 - rec.hitPoint;
	// calculate the areas and factors (order of parameters doesn't matter):
	Float a = Cross(p0 - p1, p0 - p2).Length(); // main triangle area a
	Float a1 = Cross(f2, f3).Length() / a; // p1's triangle area / a
	Float a2 = Cross(f3, f1).Length() / a; // p2's triangle area / a 
	Float a3 = Cross(f1, f2).Length() / a; // p3's triangle area / a
	// find the uv corresponding to point f (uv1/uv2/uv3 are associated to p1/p2/p3):
	rec.u = a1 * uv[0].x + a2 * uv[1].x + a3 * uv[2].x;
	rec.v = a1 * uv[0].y + a2 * uv[1].y + a3 * uv[2].y;

	//Point2f uv[3];
	//
	//float i = (-(rec.hitPoint.x - p1.x) * (p2.y - p1.y) + (rec.hitPoint.y - p1.y)*(p2.x - p1.x)) /
	//	(-(p0.x - p1.x)*(p2.y - p1.y) + (p0.y - p1.y)*(p2.x - p1.x));
	//float j = (-(rec.hitPoint.x - p2.x) * (p0.y - p2.y) + (rec.hitPoint.y - p2.y)*(p0.x - p2.x)) /
	//	(-(p1.x - p2.x)*(p0.y - p2.y) + (p1.y - p2.y)*(p0.x - p2.x));
	//float k = 1 - i - j;
	//
	//GetUV(uv);
	//rec.u = i * uv[0].x + j * uv[1].x + k * uv[2].x;
	//rec.v = i * uv[0].y + j * uv[1].y + k * uv[2].y;
	return true;
}

std::vector<std::shared_ptr<Shape>> GetMeshTriangles(std::shared_ptr<Mesh> mesh) {
	std::vector<std::shared_ptr<Shape>> triangles;
	triangles.reserve(mesh->trianglesNum);
	//for (int i = 0; i < mesh->verticesNum; i++) {
	//	std::cout << mesh->vertices[i] << std::endl;
	//}
	//for (int i = 0; i < mesh->verticesNum; i++) {
	//	std::cout << mesh->uvs[i] << std::endl;
	//}
	for (int i = 0; i < mesh->trianglesNum; ++i)
		triangles.push_back(std::make_shared<Triangle>(mesh, i, mesh->material));
	return triangles;
}