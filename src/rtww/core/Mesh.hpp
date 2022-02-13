#pragma once
#include "Transform.hpp"

class Mesh {
public:
	Mesh(std::shared_ptr<Transform> transform, int tNum, const int* vIndices,
		int vNum, const Point3f* v, const Vector3f* n, const Point2f* uv, std::shared_ptr<Material> material)
		: material(material), trianglesNum(tNum), verticesNum(vNum), vertexIndices(vIndices, vIndices + 3 * tNum) {

		vertices.reset(new Point3f[vNum]);
		auto o2w = transform->GetObject2WorldMatrix();
		for (int i = 0; i < vNum; ++i) vertices[i] = o2w(v[i]);

		if (uv) {
			uvs.reset(new Point2f[vNum]);
			memcpy(uvs.get(), uv, vNum * sizeof(Float));
		}

		if (n) {
			normals.reset(new Vector3f[vNum]);
			for (int i = 0; i < vNum; ++i) normals[i] = o2w(n[i]);
		}
	}

public:
	const int trianglesNum, verticesNum;
	std::unique_ptr<Point3f[]> vertices;
	std::unique_ptr<Point2f[]> uvs;
	std::unique_ptr<Vector3f[]> normals;
	std::vector<int> vertexIndices;
	std::shared_ptr<Material> material;
};