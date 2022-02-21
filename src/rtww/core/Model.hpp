#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "Mesh.hpp"

class Model {
public:
	Model(const char* filename, std::shared_ptr<Transform> t) : transform(t){
		Load(filename);
	}

	bool Load(const char* filename) {
		tinyobj::ObjReader reader;

		if (!reader.ParseFromFile(filename)) {
			if (!reader.Error().empty())
				std::cerr << "Load Model Error: " << reader.Error();
			return false;
		}

		if (!reader.Warning().empty())
			std::cout << "Load Model Warning: " << reader.Warning();

		auto& attrib = reader.GetAttrib();
		auto& shapes = reader.GetShapes();
		auto& materials = reader.GetMaterials();

		for (const auto& shape : shapes) {
			int verticesNum = static_cast<int>(shape.mesh.indices.size());
			int trianglesNum = verticesNum / 3;
			Point3f* vertices = (Point3f*)malloc(sizeof(Point3f) * verticesNum);
			Point2f* uvs = (Point2f*)malloc(sizeof(Point2f) * verticesNum);
			Vector3f* normals = (Vector3f*)malloc(sizeof(Vector3f) * verticesNum);
			int* vertexIndices = (int*)malloc(sizeof(int) * verticesNum);
			int offset = 0;

			for (const auto& index : shape.mesh.indices) {
				vertices[offset] = Point3f(attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]);
				uvs[offset] = Point2f(attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]);
				normals[offset] = Vector3f(attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2]);
				vertexIndices[offset] = offset;
				++offset;
			}

			meshes.push_back(std::make_shared<Mesh>(transform, trianglesNum, vertexIndices, verticesNum, 
				vertices, normals, uvs, std::make_shared<Lambertian>(Color(0.89, 0.89, 0.89))));
		}
	}

public:
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::shared_ptr<Transform> transform;
};